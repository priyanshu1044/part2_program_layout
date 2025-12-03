; Recursive Factorial Program
; Calculates Factorial of 5 (5! = 120)

; Initialize Stack Pointer
MOVI R5, 0xFF   ; SP = 0xFF (Low byte) - wait, SP is 16-bit? 
                ; My MOVI is 16-bit immediate.
                ; Let's set SP to 0xFF00 (Start of MMIO area, growing down)
                ; Or just 0xF000 to be safe.
MOVI R0, 0xF0
MOVI R1, 0x00
; Wait, MOVI R, Imm takes 16-bit imm.
; MOVI R5, 0xF000
MOVI R5, 61440 ; 0xF000

; Main Driver
MOVI R0, 5      ; Calculate 5!
PUSH R0         ; Push argument n
CALL FACTORIAL  ; Call function
POP R1          ; Pop argument (cleanup)
; Result is in R0 (Convention)

; Print Result (120 = 'x')
OUT 0xFF01, R0  ; Print as number
HALT

FACTORIAL:
    ; Stack Frame:
    ; [SP+2] = Return Address
    ; [SP+4] = Argument n
    
    ; Prologue
    PUSH R1         ; Save R1 (callee-saved)
    
    ; Load Argument n
    ; We don't have relative stack addressing (LOAD R, [SP+k])
    ; We only have POP or direct address.
    ; This makes standard stack frames hard.
    ; Let's use a simpler convention for this simple CPU.
    ; Argument in R0. Return in R0.
    ; Caller saves R0 if needed.
    
    ; RESTARTING DESIGN FOR SIMPLE CPU
    ; Argument: R0
    ; Return: R0
    ; Clobbers: R1
    
    ; Check Base Case: if n <= 1 return 1
    MOVI R1, 1
    CMP R0, R1
    JZ BASE_CASE    ; If R0 == 1
    ; Wait, CMP sets Z if equal. What if R0 < 1? (0)
    ; My CMP does R1 - R2.
    ; If R0=0, R1=1, Res = -1 (Not Zero).
    ; I don't have JLE.
    ; But for factorial, input >= 1 usually.
    ; Let's assume n >= 1.
    
    ; Recursive Step: n * factorial(n-1)
    PUSH R0         ; Save n
    MOVI R1, 1
    SUB R0, R1      ; n = n - 1
    CALL FACTORIAL  ; factorial(n-1)
    
    ; R0 has result of factorial(n-1)
    POP R1          ; Restore n (into R1)
    
    ; Multiply R0 * R1
    ; Wait, I DON'T HAVE MUL INSTRUCTION!
    ; The prompt asked for "simple multiplication" or factorial.
    ; I need to implement MUL using ADD loop.
    ; Or I can add MUL to ISA.
    ; Adding MUL to ISA is easier and cleaner.
    ; Let's add MUL.
    
    ; PAUSE: I need to add MUL to ISA first.
    ; The user said "Design and implement a complete Software CPU".
    ; Adding MUL is valid.
    
    ; Let's go back and add MUL.
    ; Opcode 0x11.
    
    ; Temporary Halt to add MUL.
    HALT

BASE_CASE:
    MOVI R0, 1
    POP R1          ; Restore R1
    RET
