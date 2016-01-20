syntax on
filetype on
set nocompatible
set laststatus=2
set statusline=%h%F%m%r%=[%l:%c(%p%%)]
set ts=4
set nonu
set autoindent
set cindent
set shiftwidth=4

set background=dark
set backspace=eol,start,indent
set history=1000
set fileencodings=utf-8,euc-kr

set hlsearch
set cursorline
set tags+=/work/kernel-mds2450-3.0.22/tags

autocmd BufReadPost *
    \ if line("'\"") > 0 && line("'\"") <= line("$") |
    \   exe "normal g`\"" |
    \ endif

map <F3> <c-w><c-w>
map <F4> :Tlist<cr>
map <F5> :BufExplorer<cr>
map <F6> <esc> :w<cr> :make<cr> :ccl<cr> :cw<cr>

