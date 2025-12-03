; Recursive Factorial Program
; Calculates Factorial of 5 (5! = 120)

; Initialize Stack Pointer to 0xF000 (61440)
MOVI SP, 61440

; Main Driver
MOVI R0, 5      ; Calculate 5!
PUSH R0         ; Push argument n
CALL FACTORIAL  ; Call function
POP R1          ; Pop argument (cleanup)
; Result is in R0

; Print Result (120 = 'x')
OUT 0xFF01, R0  ; Print as number
MOVI R1, 10     ; Newline
OUT 0xFF00, R1
HALT

FACTORIAL:
    ; Argument is on Stack: [SP+2] (because Return Address is 2 bytes at [SP])
    ; But we don't have SP-relative load.
    ; So we will use a Register Passing Convention for simplicity in this CPU.
    ; Argument: R0
    ; Return: R0
    ; Clobbers: R1
    
    ; Prologue
    PUSH R1         ; Save R1
    
    ; Base Case: if R0 <= 1 return 1
    MOVI R1, 1
    CMP R0, R1
    JZ BASE_CASE    ; If R0 == 1 (or 0 if we assume unsigned)
    
    ; Recursive Step: n * factorial(n-1)
    PUSH R0         ; Save n
    MOVI R1, 1
    SUB R0, R1      ; n = n - 1
    CALL FACTORIAL  ; factorial(n-1)
    
    ; R0 has result of factorial(n-1)
    POP R1          ; Restore n (into R1)
    
    ; Multiply R0 * R1 (Result * n)
    MUL R0, R1
    
    ; Epilogue
    POP R1          ; Restore R1
    RET

BASE_CASE:
    MOVI R0, 1
    POP R1          ; Restore R1
    RET
