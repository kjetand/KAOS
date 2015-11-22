;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Global Descriptor Table (GDT)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

gdt_data: 
    dd 0  ; null descriptor
    dd 0
 
; gdt code:       ; Code descriptor
    dw 0FFFFh     ; Limit low
    dw 0          ; Base low
    db 0          ; Base middle
    db 10011010b  ; Access
    db 11001111b  ; Granularity
    db 0          ; Base high
 
; GDT data:       ; Data descriptor
    dw 0FFFFh     ; Limit low (Same as code)
    dw 0          ; Base low
    db 0          ; Base middle
    db 10010010b  ; Access
    db 11001111b  ; Granularity
    db 0          ; Base high
 
end_of_gdt:
toc: 
    dw end_of_gdt - gdt_data - 1  ; Limit (Size of GDT)
    dd gdt_data                   ; Base of GDT
