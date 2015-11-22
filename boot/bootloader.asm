[org 0x7C00]

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; STAGE 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[bits 16]

start: jmp stage1

DONE_MSG: db "DONE", 0
FAIL_MSG: db "FAIL", 0
FLOPPY_ERROR_MSG: db "Floppy error!", 0

STAGE1_STACK_MSG: db "Setting up bootloader stack.............. ", 0
STAGE1_LOAD_MSG: db "Loading OS from floppy................... ", 0

stage1:
    ; Clear segment registers
    cli         ; Clear interrupts
    xor ax, ax  ; ax = 0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Setup bootloader stack
    mov ax, 0x9000
    mov ss, ax      ; Set top of stack to 0x9000
    mov sp, 0xFFFF  ; Set bottom of stack to 0xFFFF
    sti
    call clear_screen
    mov si, STAGE1_STACK_MSG
    call print
    mov si, DONE_MSG
    call println
    
    ; Load rest of bootloader and OS into memory
    mov si, STAGE1_LOAD_MSG
    call print
    call reset_floppy  ; Reset floppy disk controller
    call load_os       ; Read OS from floppy
    mov ax, [0x7C00 + 512]
    cmp ax, 0xBEEF
    jne stage1_error   ; If magic number is not correct = ERROR
    mov si, DONE_MSG   ; Else print DONE
    call println

    jmp stage2

reset_floppy:
    mov ah, 0         ; Function 0
    mov dl, 0         ; Reset drive 0 = floppy drive
    int 0x13
    jc reset_floppy   ; If error, try again
    ret

load_os:
    mov bx, 0x7C00 + 512  ; Where to load the OS
    mov al, 10            ; Number of sectors to read
    mov ch, 0             ; Cylinder number
    mov cl, 2             ; Sector number
    mov dh, 0             ; Head number
    mov dl, 0             ; Drive number = floppy
    mov ah, 0x02          ; Function nr.2
    int 0x13
    jc load_os            ; If error, try again
    ret

print:
    lodsb          ; Load byte from si to al and increment
    xor bx, bx     ; Set bx to 0
    or al, al      ; Is al 0?
    jz print_done
    mov ah, 0x0E   ; BIOS putc function
    int 0x10       ; BIOS graphics interrupt
    jmp print

print_done:
    ret

NEW_LINE: db 0x0D, 0x0A, 0

println:
    call print
    mov si, NEW_LINE
    call print
    ret

clear_screen:
    mov ah, 0x0
    mov al, 0x3
    int 0x10
    ret

stage1_error:
    mov si, FAIL_MSG
    call println
    cli
    hlt

times 510 - ($-$$) db 0
dw 0xAA55

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; STAGE 2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

dw 0xBEEF  ; Magic number

%include "boot/gdt.inc"

STAGE2_GDT_MSG: db "Installing GDT........................... ", 0
STAGE2_A20_MSG: db "Enabling 32-bit addressing............... ", 0
STAGE2_PM_MSG: db "Entering protected mode.................. ", 0

stage2:
    ; Install Global Descriptor Table (GDT)
    mov si, STAGE2_GDT_MSG
    call print
    call install_gdt
    mov si, DONE_MSG
    call println

    ; Enable A20
    mov si, STAGE2_A20_MSG
    call print
    call enable_a20

    ; Enter protected mode
    mov si, STAGE2_PM_MSG
    call print
    mov si, DONE_MSG
    call println
    cli
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp 0x08:stage3

install_gdt:
    cli         ; Clear interrupts
    pusha       ; Save registers
    lgdt [toc]  ; Load GDT into GDTR
    sti         ; Enable interrupts
    popa        ; Restore registers
    ret

enable_a20:
    call test_a20
    cmp ax, 1
    je enable_a20_return
    call enable_a20_bios

    call test_a20
    cmp ax, 1
    je enable_a20_return
    call enable_a20_8042

    call test_a20
    cmp ax, 1
    je enable_a20_return
    call enable_a20_fast_gate

    call test_a20
    cmp ax, 1
    je enable_a20_return
    jmp enable_a20_error

enable_a20_return:
    mov si, DONE_MSG
    call println
    ret

enable_a20_error:
    mov si, FAIL_MSG
    call println
    cli
    hlt

; Approach 1: Through BIOS interrupt
enable_a20_bios:
    mov ax, 0x2401
    int 0x15
    ret

; Approach 2: Through keyboard controller 8042
enable_a20_8042:
    ; Send read output port command
    mov al, 0xD0
    out 0x64, al
    call wait_8042_output
 
    ; Read input buffer and store on stack. This is the data read from the output port
    in al, 0x60
    push eax
    call wait_8042_input
 
    ; Send write output port command
    mov al, 0xD1
    out 0x64, al
    call wait_8042_input
 
    ; Pop the output port data from stack and set bit 1 (A20) to enable
    pop eax
    or al, 2
    out 0x60, al
    ret

; Approach 3: Through IO-port 0x92 (work only on some chipsets) 
enable_a20_fast_gate:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

; Test A20: ax is set to 1 if A20 is enabled, else 0
test_a20:
    pushf
    push ds
    push es
    push di
    push si
    cli

    ; Set es:di to 0000:0500
    xor ax, ax
    mov es, ax
    mov di, 0x0500

    ; Set ds:si to FFFF:0510
    mov ax, 0xFFFF
    mov ds, ax
    mov si, 0x0510

    ; Store values as they will be restored
    mov al, [es:di]
    push ax
    mov al, [ds:si]
    push ax

    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF
    cmp byte [es:di], 0xFF

    ; Restore values
    pop ax
    mov byte [ds:si], al
    pop ax
    mov byte [es:di], al

    mov ax, 0
    je test_a20_return  ; Return 0 if memory wraps around
    mov ax, 1           ; else return 1.

test_a20_return:
    pop si
    pop di
    pop es
    pop ds
    popf
    ret

; Wait for keyboard controller to be ready to receive commands
wait_8042_input:
    in al, 0x64            ; Read status register into al
    test al, 2             ; Test input buffer status bit
    jnz wait_8042_input
    ret

; Wait for keyboard controller to have data ready
wait_8042_output:
    in al, 0x64          ; Read status register into al
    test al, 1           ; Test output buffer status bit
    jnz wait_8042_output
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; STAGE 3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[bits 32]

stage3:
    ; Set segment registers to data descriptor in GDT
    mov ax, 0x10  ; 0x10 is address of data descriptor
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Setting up kernel stack
    mov esp, 0x90000
    mov ebp, esp

    ; Jump to kernel
    jmp 0x7C00 + 1024
