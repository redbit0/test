;;=============================================================================
;; 
;;
;;=============================================================================
.CODE


; void _break();
;------------------------------------------------------------------------------
_break PROC
	int 3
	ret
_break ENDP

;;=============================================================================
;; MSR stuff
;;=============================================================================

; out parameter 로 호출하기
; 32비트와 64 비트 c 함수 시그너처를 동일하게 하려면 eax 에 64비트를 담아 리턴이 안되므로
; out 파라미터를 사용하도록 C 함수를 만든다. 
; 
; 아니면 edx:eax 를 그냥 리턴하게 하는 asm 함수를 만들고, 
; 그걸 다시 64비트 변수로 리턴하게 하는 asm 함수/매크로를 만들어서 처리하고, 
; C 함수는 그냥 64 비트 변수를 리턴하게 작성하면 됨 (32 비트코드에서 64 비트 리턴하는 함수처럼)
;
; out parameter 를 컴파일러에서는 어떻게 만들어내는지 확인해볼 필요 있음

;extern "C" BOOLEAN MsrRead2(IN ULONG32 ecx_value, OUT ULONG32* msr_high, OUT ULONG32* msr_low);
MsrRead2 proc
	push rdx		; save second out param
	
	rdmsr			; MSR[ecx] -> edx:eax
	mov r10d, edx
	pop rdx
	mov dword ptr [rdx], r10d
	
	mov dword ptr [r8], eax
	mov eax, 1
	ret
MsrRead2 endp

; MsrRead (ULONG32 ecx_value (rcx));
;------------------------------------------------------------------------------
MsrRead PROC
;	xor		rax, rax
	rdmsr				; MSR[ecx] --> edx:eax
	shl		rdx, 32
	or		rax, rdx
	ret
MsrRead ENDP

; MsrWrite (ULONG32 ecx_value (rcx), ULONG32 eax_value (rdx), ULONG32 edx_value (r8));
;------------------------------------------------------------------------------
MsrWrite PROC
	mov		rax, rdx
	mov		rdx, r8
	xor		r8, r8
	wrmsr
	mov		rax, r8		; r8 will be set to STATUS_UNSUCCESSFUL if there is a fault
	ret
MsrWrite ENDP




END




