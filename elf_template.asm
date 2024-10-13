[bits 64]

; Virtual address at which we request our ELF to be mapped into memory. This is
; somewhat arbitrary, but we don't want it to be 0, and it's probably good to
; keep it page-aligned.
file_load_va: equ 4096 * 40

; ELF header

; Signature
db 0x7f, 'E', 'L', 'F'
; "Class" = 2, 64-bit
db 2
; Endianness = 1, little
db 1
; ELF version = 1
db 1
; OS ABI, unused, should be 0
db 0
; Extended ABI byte + 7 bytes padding. Leave as 0, it's ignored
dq 0
; ELF file type. 2 = executable
dw 2
; Target architecture. 0x3e = x86_64
dw 0x3e
; Additional ELF version stuff. Leave as 1.
dd 1
; Entry point address.
dq _start + file_load_va
; Program header offset. We'll put it immediately after the ELF header.
dq program_headers_start
; Section header offset. We'll put it after the program headers.
dq 0
; More flags. Not used, as far as I know.
dd 0
; Size of this header, 64 bytes.
dw 64
; Size of a program header entry.
dw 0x38
; Number of program header entries.
dw 1
; Size of a section header entry.
dw 0x40
; Number of section header entries
dw 0
; Index of the section containing the string table with section names
dw 0


program_headers_start:
; First field: The program header type. 1 = loadable segment.
dd 1
; Program header flags. 5 = Not writable. (bits 0, 1, and 2 = executable,
; writable, and readable, respectively)
dd 5
; The offset of the loadable segment in the file. This will contain the entire
; file, so make it 0.
dq 0
; The VA to place the segment at.
dq file_load_va
; The "phyiscal address". Don't think it's used, set to same as VA.
dq file_load_va
; The size of the segment in the file. It ends at the string table.
dq file_end
; The size of the segment in memory.
dq file_end
; The alignment of the segment
dq 0x200000

; function that prepares arguments for entrypoint and exits after done
_start:
  mov rdi, [rsp]
  lea rsi, [rsp + 8]
  lea rdx, [rsp + 8*(rdi + 1)]
  call entry_point
  mov rax, 60
  mov rdi, 0
  syscall

file_end:
