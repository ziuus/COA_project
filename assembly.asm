.data
    # Simulated values (Scaled by 100 for decimals)
    # Example: 34.55C -> 3455, 32.10C -> 3210
    current_temp: .word 3455      
    threshold:    .word 3300      
    
    # Strings for output (Simulating Serial Monitor)
    msg_normal:   .string "STATUS: NORMAL\n"
    msg_critical: .string "STATUS: !!! CRITICAL !!!\n"

.text
.global main

main:
    # 1. Load the simulated values into registers
    la t0, current_temp    # Load address of current_temp
    lw s0, 0(t0)           # s0 = current temperature (3455)
    
    la t1, threshold       # Load address of threshold
    lw s1, 0(t1)           # s1 = threshold (3300)

    # 2. THE CORE LOGIC (Branching)
    # If s0 < s1 (current < threshold), jump to 'safe'
    blt s0, s1, safe       

critical:
    # 3. Handle Critical State (Output Message)
    la a0, msg_critical    # Load address of critical message
    li a7, 4               # Environment call 4: print string
    ecall                  # Execute call
    j end                  # Jump to end

safe:
    # 4. Handle Normal State (Output Message)
    la a0, msg_normal      # Load address of normal message
    li a7, 4               # Environment call 4: print string
    ecall                  # Execute call

end:
    # Exit program
    li a7, 10              # Environment call 10: exit
    ecall