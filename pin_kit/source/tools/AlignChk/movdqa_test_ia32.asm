.386
.XMM
.model flat, c

.code

movdqa_test PROC xmm_reg_ptr:DWORD
  mov    eax,   xmm_reg_ptr
  movdqa xmm0,  [eax]
  movdqa xmm0,  [eax+1]	
  RET
movdqa_test ENDP


end
