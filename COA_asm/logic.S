.section .text
.global check_temp_asm

# RISC-V Calling Convention:
# a0 = current_temp (passed from C++)
# a1 = threshold    (passed from C++)
# Result is returned in a0

check_temp_asm:
    # Use the Branch if Less Than instruction
    blt a0, a1, safe_state  # If current_temp < threshold, jump to 'safe_state'
    
    # Critical State
    li a0, 1                # Load 1 into return register (Warning)
    ret                     # Return to C++ code

safe_state:
    li a0, 0                # Load 0 into return register (Normal)
    ret                     # Return to C++ code