;;=============================================================================
;; 
;;
;;=============================================================================
.686
.model flat, StdCall
OPTION CASEMAP:NONE
.CODE

; void _break();
;------------------------------------------------------------------------------
_break proc stdcall 
	int 3
	ret
_break endp


;;=============================================================================
;; MSR stuff
;;=============================================================================

; ULONG64 MsrRead(IN ULONG32 ecx_value);
;MsrRead PROC StdCall _ecx_value
	;mov		ecx, _ecx_value
	;rdmsr				; MSR[ecx] --> edx:eax
	;ret
;MsrRead ENDP
;

; MsrRead (ULONG32 ecx_value (rcx));
;------------------------------------------------------------------------------
;MsrRead PROC
;;	xor		rax, rax
	;rdmsr				; MSR[ecx] --> edx:eax
	;shl		rdx, 32
	;or		rax, rdx
	;ret
;MsrRead ENDP
;

; MsrWrite (ULONG32 ecx_value (rcx), ULONG32 eax_value (rdx), ULONG32 edx_value (r8));
;------------------------------------------------------------------------------
;MsrWrite PROC
	;mov		rax, rdx
	;mov		rdx, r8
	;xor		r8, r8
	;wrmsr
	;mov		rax, r8		; r8 will be set to STATUS_UNSUCCESSFUL if there is a fault
	;ret
;MsrWrite ENDP
;



END




