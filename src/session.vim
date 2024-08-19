let SessionLoad = 1
if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
inoremap <silent> <Plug>NERDCommenterInsert  <BS>:call NERDComment('i', 'insert')
imap <C-G>S <Plug>ISurround
imap <C-G>s <Plug>Isurround
imap <C-S> <Plug>Isurround
inoremap <silent> <Plug>(table-mode-tableize) |:call tablemode#TableizeInsertMode()a
inoremap <silent> <expr> <Plug>delimitMateS-BS delimitMate#WithinEmptyPair() ? "\<Del>" : "\<S-BS>"
inoremap <silent> <Plug>delimitMateBS =delimitMate#BS()
inoremap <silent> <Plug>(complete_parameter#overload_up) :call cmp#overload_next(0)
inoremap <silent> <Plug>(complete_parameter#overload_down) :call cmp#overload_next(1)
inoremap <silent> <Plug>(complete_parameter#goto_previous_parameter) :call cmp#goto_next_param(0)
inoremap <silent> <Plug>(complete_parameter#goto_next_parameter) :call cmp#goto_next_param(1)
imap <silent> <C-G>% <Plug>(matchup-c_g%)
inoremap <silent> <Plug>(matchup-c_g%) :call matchup#motion#insert_mode()
imap <expr> <M-/> neosnippet#expandable() ? "\<Plug>(neosnippet_expand)" : ""
inoremap <expr> <PageUp> pumvisible() ? "\<PageUp>\\" : "\<PageUp>"
inoremap <expr> <PageDown> pumvisible() ? "\<PageDown>\\" : "\<PageDown>"
inoremap <expr> <Up> pumvisible() ? "\" : "\<Up>"
inoremap <expr> <Down> pumvisible() ? "\" : "\<Down>"
inoremap <expr> <S-Tab> pumvisible() ? "\<Up>" : ""
cnoremap <C-S> w
inoremap <S-CR> o
inoremap <silent> <C-S-Up> :m .-2==gi
inoremap <silent> <C-S-Down> :m .+1==gi
map  <Plug>(SmoothieBackwards)
map  <Plug>(SmoothieDownwards)
map  <Plug>(SmoothieForwards)
nnoremap <silent>  :call SpaceVim#plugins#ctrlg#display()
xnoremap 	 >gv
vnoremap  :w
nnoremap  :w
map  <Plug>(SmoothieUpwards)
nmap  xd  [SPC]xd[SPC]
nmap  xa  [SPC]xa[SPC]
vmap   [SPC]
nmap   [SPC]
omap <silent> % <Plug>(matchup-%)
xmap <silent> % <Plug>(matchup-%)
nmap <silent> % <Plug>(matchup-%)
nnoremap <silent> ,  :silent! keeppatterns %substitute/\s\+$//e
nnoremap < <<_
xnoremap < <gv
nnoremap > >>_
xnoremap > >gv
imap <expr> ¯ neosnippet#expandable() ? "\<Plug>(neosnippet_expand)" : ""
vmap <silent> J <Plug>(jplus)
nmap <silent> J <Plug>(jplus)
xmap S <Plug>VSurround
xmap V <Plug>(expand_region_shrink)
nnoremap <silent> [SPC]ao :call SpaceVim#plugins#todo#list()
omap <silent> [% <Plug>(matchup-[%)
xmap <silent> [% <Plug>(matchup-[%)
nmap <silent> [% <Plug>(matchup-[%)
nnoremap <silent> [SPC]as :Startify | doautocmd WinEnter
nnoremap <silent> [SPC]tmT :if &laststatus == 2 | let &laststatus = 0 | else | let &laststatus = 2 | endif
nnoremap <silent> [SPC]tmp :call SpaceVim#layers#core#statusline#toggle_section("cursorpos")
nnoremap <silent> [SPC]tmt :call SpaceVim#layers#core#statusline#toggle_section("time")
nnoremap <silent> [SPC]tmi :call SpaceVim#layers#core#statusline#toggle_section("input method")
nnoremap <silent> [SPC]tmd :call SpaceVim#layers#core#statusline#toggle_section("date")
nnoremap <silent> [SPC]tmb :call SpaceVim#layers#core#statusline#toggle_section("battery status")
nnoremap <silent> [SPC]tmM :call SpaceVim#layers#core#statusline#toggle_section("major mode")
nnoremap <silent> [SPC]tmm :call SpaceVim#layers#core#statusline#toggle_section("minor mode lighters")
nmap <silent> [SPC]; <Plug>CommentOperator
xmap <silent> [SPC]cP <Plug>CommentParagraphs
nmap <silent> [SPC]cP <Plug>CommentParagraphs
xmap <silent> [SPC]cp <Plug>CommentParagraphsInvert
nmap <silent> [SPC]cp <Plug>CommentParagraphsInvert
xmap <silent> [SPC]cT <Plug>CommentToLine
nmap <silent> [SPC]cT <Plug>CommentToLine
xmap <silent> [SPC]ct <Plug>CommentToLineInvert
nmap <silent> [SPC]ct <Plug>CommentToLineInvert
xmap <silent> [SPC]c$ <Plug>NERDCommenterToEOL
nmap <silent> [SPC]c$ <Plug>NERDCommenterToEOL
xmap <silent> [SPC]cY <Plug>NERDCommenterYank
nmap <silent> [SPC]cY <Plug>NERDCommenterYank
xmap <silent> [SPC]cy <Plug>CommenterInvertYank
nmap <silent> [SPC]cy <Plug>CommenterInvertYank
xmap <silent> [SPC]cs <Plug>NERDCommenterSexy
nmap <silent> [SPC]cs <Plug>NERDCommenterSexy
xmap <silent> [SPC]cv <Plug>NERDCommenterInvertgv
nmap <silent> [SPC]cv <Plug>NERDCommenterInvertgv
xmap <silent> [SPC]cu <Plug>NERDCommenterUncomment
nmap <silent> [SPC]cu <Plug>NERDCommenterUncomment
xmap <silent> [SPC]cL <Plug>NERDCommenterComment
nmap <silent> [SPC]cL <Plug>NERDCommenterComment
xmap <silent> [SPC]cl <Plug>NERDCommenterInvert
nmap <silent> [SPC]cl <Plug>NERDCommenterInvert
xmap <silent> [SPC]ca <Plug>NERDCommenterAltDelims
nmap <silent> [SPC]ca <Plug>NERDCommenterAltDelims
nnoremap <silent> [SPC]qr :
nnoremap <silent> [SPC]qR :
nnoremap <silent> [SPC]qQ :qa!
nnoremap <silent> [SPC]qq :qa
nnoremap <silent> [SPC]p/ :Grepper
nnoremap <silent> [SPC]pp :call SpaceVim#plugins#projectmanager#list()
nnoremap <silent> [SPC]pk :call SpaceVim#plugins#projectmanager#kill_project()
nnoremap <silent> [SPC]ptr :call SpaceVim#plugins#runner#run_task(SpaceVim#plugins#tasks#get())
nnoremap <silent> [SPC]ptc :call SpaceVim#plugins#runner#clear_tasks()
nnoremap <silent> [SPC]ptl :call SpaceVim#plugins#tasks#list()
nnoremap <silent> [SPC]pte :call SpaceVim#plugins#tasks#edit()
nnoremap <silent> [SPC]fvd :SPConfig
nnoremap <silent> [SPC]fvv :let @+=g:spacevim_version | echo g:spacevim_version
xmap <silent> [SPC]fY <Plug>YankGitRemoteURL
nmap <silent> [SPC]fY <Plug>YankGitRemoteURL
nnoremap <silent> [SPC]fy :call SpaceVim#util#CopyToClipboard()
nnoremap <silent> [SPC]bt :NERDTree %
nnoremap <silent> [SPC]fo :NERDTreeFind
nnoremap <silent> [SPC]fT :NERDTree
nnoremap <silent> [SPC]ft :NERDTreeToggle
nnoremap <silent> [SPC]f/ :call SpaceVim#plugins#find#open()
nnoremap <silent> [SPC]fCu :update | e ++ff=dos | setlocal ff=unix | w
nnoremap <silent> [SPC]fCd :update | e ++ff=dos | w
nnoremap <silent> [SPC]fb :BookmarkShowAll
nnoremap <silent> [SPC]bNn :enew
nnoremap <silent> [SPC]bNl :rightbelow vertical new
nnoremap <silent> [SPC]bNk :new
nnoremap <silent> [SPC]bNj :rightbelow new
nnoremap <silent> [SPC]bNh :topleft vertical new
nnoremap <silent> [SPC]bw :setl readonly!
nnoremap <silent> [SPC]bY :normal! ggVG"+y``
nnoremap <silent> [SPC]bP :normal! ggdG"+P
nnoremap <silent> [SPC]bh :Startify
nnoremap <silent> [SPC]bc :call SpaceVim#mapping#clear_saved_buffers()
nnoremap <silent> [SPC]b<C-S-D> :call SpaceVim#mapping#kill_buffer_expr()
nnoremap <silent> [SPC]b<C-D> :call SpaceVim#mapping#clear_buffers()
nnoremap <silent> [SPC]b :call SpaceVim#mapping#kill_buffer_expr()
nnoremap <silent> [SPC]bD :call SpaceVim#mapping#kill_visible_buffer_choosewin()
nnoremap <silent> [SPC]bd :call SpaceVim#mapping#close_current_buffer()
nnoremap <silent> [SPC]hG :call SpaceVim#plugins#helpgrep#help(expand("<cword>"))
nnoremap <silent> [SPC]hg :call SpaceVim#plugins#helpgrep#help()
nnoremap <silent> [SPC]	 :try | b# | catch | endtry
nnoremap <silent> [SPC]jn i
nnoremap <silent> [SPC]jf 	
nnoremap <silent> [SPC]jb 
nnoremap <silent> [SPC]j$ m`g_
nnoremap <silent> [SPC]j0 m`^
nnoremap <silent> [SPC]hk :LeaderGuide "[KEYs]"
nnoremap <silent> [SPC]hL :SPRuntimeLog
nnoremap <silent> [SPC]hl :SPLayer -l
nnoremap <silent> [SPC]hI :call SpaceVim#issue#report()
nnoremap <silent> [SPC]fS :wall
nnoremap <silent> [p P
nnoremap <silent> [t :tabprevious
nnoremap <silent> [l :lprevious
nnoremap <silent> [b :bN | stopinsert
nnoremap <silent> [e :execute 'move -1-'. v:count1
nnoremap <silent> [  :put! =repeat(nr2char(10), v:count1)
nnoremap <silent> [SPC]tW :call SpaceVim#layers#core#statusline#toggle_mode("wrapline")
nnoremap <silent> [SPC]tl :setlocal list!
nnoremap <silent> [SPC]tP :DelimitMateSwitch
nnoremap <silent> [SPC]tp :call SpaceVim#layers#core#statusline#toggle_mode("paste-mode")
nnoremap <silent> [SPC]tS :call SpaceVim#layers#core#statusline#toggle_mode("spell-checking")
nnoremap <silent> [SPC]tw :call SpaceVim#layers#core#statusline#toggle_mode("whitespace")
nnoremap <silent> [SPC]thc :set cursorcolumn!
nnoremap <silent> [SPC]tf :call SpaceVim#layers#core#statusline#toggle_mode("fill-column-indicator")
nnoremap <silent> [SPC]tt :call SpaceVim#plugins#tabmanager#open()
nnoremap <silent> [SPC]ta :call SpaceVim#layers#autocomplete#toggle_deoplete()
nnoremap <silent> [SPC]t8 :call SpaceVim#layers#core#statusline#toggle_mode("hi-characters-for-long-lines")
nmap <silent> [SPC]jq <Plug>(easymotion-overwin-line)
nmap <silent> [SPC]jw <Plug>(easymotion-overwin-w)
nmap <silent> [SPC]jv <Plug>(easymotion-overwin-line)
xmap <silent> [SPC]jl <Plug>(better-easymotion-overwin-line)
nmap <silent> [SPC]jl <Plug>(better-easymotion-overwin-line)
nmap <silent> [SPC]jJ <Plug>(easymotion-overwin-f2)
xmap <silent> [SPC]jj <Plug>(better-easymotion-overwin-f)
nmap <silent> [SPC]jj <Plug>(better-easymotion-overwin-f)
nnoremap <silent> [SPC]jk j==
nnoremap <silent> [SPC]jm :SplitjoinSplit
nnoremap <silent> [SPC]jo :SplitjoinJoin
xmap <silent> [SPC]xgp <Plug>(grammarous-move-to-previous-error)
nmap <silent> [SPC]xgp <Plug>(grammarous-move-to-previous-error)
xmap <silent> [SPC]xgn <Plug>(grammarous-move-to-next-error)
nmap <silent> [SPC]xgn <Plug>(grammarous-move-to-next-error)
xmap <silent> [SPC]xlU <Plug>UniquifyCaseSenstiveLines
nmap <silent> [SPC]xlU <Plug>UniquifyCaseSenstiveLines
xmap <silent> [SPC]xlu <Plug>UniquifyIgnoreCaseLines
nmap <silent> [SPC]xlu <Plug>UniquifyIgnoreCaseLines
nnoremap <silent> [SPC]xlS :sort
nnoremap <silent> [SPC]xls :sort i
xmap <silent> [SPC]xlr <Plug>ReverseLines
nmap <silent> [SPC]xlr <Plug>ReverseLines
xmap <silent> [SPC]xld <Plug>DuplicateLines
nmap <silent> [SPC]xld <Plug>DuplicateLines
vnoremap <silent> [SPC]xwc :normal! :'<,'>s/\w\+//gn
xmap <silent> [SPC]x~ <Plug>ToggleCase
nmap <silent> [SPC]x~ <Plug>ToggleCase
xmap <silent> [SPC]xU <Plug>Uppercase
nmap <silent> [SPC]xU <Plug>Uppercase
xmap <silent> [SPC]xu <Plug>Lowercase
nmap <silent> [SPC]xu <Plug>Lowercase
nnoremap <silent> [SPC]xdw :StripWhitespace
xmap <silent> [SPC]xa[SPC] :Tabularize /\s\ze\S/l0
nmap <silent> [SPC]xa[SPC] :Tabularize /\s\ze\S/l0
xnoremap <silent> [SPC]xa| :Tabularize /[|｜]
nnoremap <silent> [SPC]xa| :Tabularize /[|｜]
xnoremap <silent> [SPC]xa¦ :Tabularize /¦
nnoremap <silent> [SPC]xa¦ :Tabularize /¦
xnoremap <silent> [SPC]xao :Tabularize /&&\|||\|\.\.\|\*\*\|<<\|>>\|\/\/\|[-+*/.%^><&|?]/l1r1
nnoremap <silent> [SPC]xao :Tabularize /&&\|||\|\.\.\|\*\*\|<<\|>>\|\/\/\|[-+*/.%^><&|?]/l1r1
xnoremap <silent> [SPC]xa= :Tabularize /===\|<=>\|\(&&\|||\|<<\|>>\|\/\/\)=\|=\~[#?]\?\|=>\|[:+/*!%^=><&|.?-]\?=[#?]\?/l1r1
nnoremap <silent> [SPC]xa= :Tabularize /===\|<=>\|\(&&\|||\|<<\|>>\|\/\/\)=\|=\~[#?]\?\|=>\|[:+/*!%^=><&|.?-]\?=[#?]\?/l1r1
xnoremap <silent> [SPC]xa; :Tabularize /;
nnoremap <silent> [SPC]xa; :Tabularize /;
xnoremap <silent> [SPC]xa: :Tabularize /:
nnoremap <silent> [SPC]xa: :Tabularize /:
xnoremap <silent> [SPC]xa. :Tabularize /\.
nnoremap <silent> [SPC]xa. :Tabularize /\.
xnoremap <silent> [SPC]xa, :Tabularize /,
nnoremap <silent> [SPC]xa, :Tabularize /,
xnoremap <silent> [SPC]xa} :Tabularize /}
nnoremap <silent> [SPC]xa} :Tabularize /}
xnoremap <silent> [SPC]xa{ :Tabularize /{
nnoremap <silent> [SPC]xa{ :Tabularize /{
xnoremap <silent> [SPC]xa] :Tabularize /]
nnoremap <silent> [SPC]xa] :Tabularize /]
xnoremap <silent> [SPC]xa[ :Tabularize /[
nnoremap <silent> [SPC]xa[ :Tabularize /[
xnoremap <silent> [SPC]xa) :Tabularize /)
nnoremap <silent> [SPC]xa) :Tabularize /)
xnoremap <silent> [SPC]xa( :Tabularize /(
nnoremap <silent> [SPC]xa( :Tabularize /(
xnoremap <silent> [SPC]xa& :Tabularize /&
nnoremap <silent> [SPC]xa& :Tabularize /&
xnoremap <silent> [SPC]xa% :Tabularize /%
nnoremap <silent> [SPC]xa% :Tabularize /%
xnoremap <silent> [SPC]xa# :Tabularize /#
nnoremap <silent> [SPC]xa# :Tabularize /#
xmap <silent> [SPC]xc <Plug>CountSelectionRegion
nmap <silent> [SPC]xc <Plug>CountSelectionRegion
nnoremap <silent> [SPC]bf :Neoformat
nnoremap <silent> [SPC]ts :call SpaceVim#layers#core#statusline#toggle_mode("syntax-checking")
nnoremap <silent> [SPC]eh :
nnoremap <silent> [SPC]xss :NeoSnippetEdit
nnoremap <silent> [SPC]hdk :call SpaceVim#plugins#help#describe_key()
nnoremap <silent> [SPC]sh :call SpaceVim#plugins#highlight#start(0)
nnoremap <silent> [SPC]sH :call SpaceVim#plugins#highlight#start(1)
nnoremap <silent> [SPC]sE :call SpaceVim#plugins#iedit#start({"selectall" : 0})
xmap <silent> [SPC]se <Plug>SpaceVim-plugin-iedit
nmap <silent> [SPC]se <Plug>SpaceVim-plugin-iedit
nnoremap <silent> [SPC]sc :call SpaceVim#plugins#searcher#clear()
nnoremap <silent> [SPC]s/ :call SpaceVim#plugins#flygrep#open({})
nnoremap <silent> [SPC]stJ :call SpaceVim#plugins#searcher#find(expand("<cword>"), "pt")
nnoremap <silent> [SPC]stj :call SpaceVim#plugins#searcher#find("", "pt")
nnoremap <silent> [SPC]stF :call SpaceVim#mapping#search#grep("t", "F")
nnoremap <silent> [SPC]stf :call SpaceVim#mapping#search#grep("t", "f")
nnoremap <silent> [SPC]stP :call SpaceVim#mapping#search#grep("t", "P")
nnoremap <silent> [SPC]stp :call SpaceVim#mapping#search#grep("t", "p")
nnoremap <silent> [SPC]stD :call SpaceVim#mapping#search#grep("t", "D")
nnoremap <silent> [SPC]std :call SpaceVim#mapping#search#grep("t", "d")
nnoremap <silent> [SPC]stB :call SpaceVim#mapping#search#grep("t", "B")
nnoremap <silent> [SPC]stb :call SpaceVim#mapping#search#grep("t", "b")
nnoremap <silent> [SPC]siJ :call SpaceVim#plugins#searcher#find(expand("<cword>"), "findstr")
nnoremap <silent> [SPC]sij :call SpaceVim#plugins#searcher#find("", "findstr")
nnoremap <silent> [SPC]siF :call SpaceVim#mapping#search#grep("i", "F")
nnoremap <silent> [SPC]sif :call SpaceVim#mapping#search#grep("i", "f")
nnoremap <silent> [SPC]siP :call SpaceVim#mapping#search#grep("i", "P")
nnoremap <silent> [SPC]sip :call SpaceVim#mapping#search#grep("i", "p")
nnoremap <silent> [SPC]siD :call SpaceVim#mapping#search#grep("i", "D")
nnoremap <silent> [SPC]sid :call SpaceVim#mapping#search#grep("i", "d")
nnoremap <silent> [SPC]siB :call SpaceVim#mapping#search#grep("i", "B")
nnoremap <silent> [SPC]sib :call SpaceVim#mapping#search#grep("i", "b")
nnoremap <silent> [SPC]srJ :call SpaceVim#plugins#searcher#find(expand("<cword>"), "rg")
nnoremap <silent> [SPC]srj :call SpaceVim#plugins#searcher#find("", "rg")
nnoremap <silent> [SPC]srF :call SpaceVim#mapping#search#grep("r", "F")
nnoremap <silent> [SPC]srf :call SpaceVim#mapping#search#grep("r", "f")
nnoremap <silent> [SPC]srP :call SpaceVim#mapping#search#grep("r", "P")
nnoremap <silent> [SPC]srp :call SpaceVim#mapping#search#grep("r", "p")
nnoremap <silent> [SPC]srD :call SpaceVim#mapping#search#grep("r", "D")
nnoremap <silent> [SPC]srd :call SpaceVim#mapping#search#grep("r", "d")
nnoremap <silent> [SPC]srB :call SpaceVim#mapping#search#grep("r", "B")
nnoremap <silent> [SPC]srb :call SpaceVim#mapping#search#grep("r", "b")
nnoremap <silent> [SPC]skJ :call SpaceVim#plugins#searcher#find(expand("<cword>"), "ack")
nnoremap <silent> [SPC]skj :call SpaceVim#plugins#searcher#find("", "ack")
nnoremap <silent> [SPC]skF :call SpaceVim#mapping#search#grep("k", "F")
nnoremap <silent> [SPC]skf :call SpaceVim#mapping#search#grep("k", "f")
nnoremap <silent> [SPC]skP :call SpaceVim#mapping#search#grep("k", "P")
nnoremap <silent> [SPC]skp :call SpaceVim#mapping#search#grep("k", "p")
nnoremap <silent> [SPC]skD :call SpaceVim#mapping#search#grep("k", "D")
nnoremap <silent> [SPC]skd :call SpaceVim#mapping#search#grep("k", "d")
nnoremap <silent> [SPC]skB :call SpaceVim#mapping#search#grep("k", "B")
nnoremap <silent> [SPC]skb :call SpaceVim#mapping#search#grep("k", "b")
nnoremap <silent> [SPC]sGF :call SpaceVim#mapping#search#grep("G", "F")
nnoremap <silent> [SPC]sGf :call SpaceVim#mapping#search#grep("G", "f")
nnoremap <silent> [SPC]sGP :call SpaceVim#mapping#search#grep("G", "P")
nnoremap <silent> [SPC]sGp :call SpaceVim#mapping#search#grep("G", "p")
nnoremap <silent> [SPC]sGD :call SpaceVim#mapping#search#grep("G", "D")
nnoremap <silent> [SPC]sGd :call SpaceVim#mapping#search#grep("G", "d")
nnoremap <silent> [SPC]sGB :call SpaceVim#mapping#search#grep("G", "B")
nnoremap <silent> [SPC]sGb :call SpaceVim#mapping#search#grep("G", "b")
nnoremap <silent> [SPC]sgJ :call SpaceVim#plugins#searcher#find(expand("<cword>"), "grep")
nnoremap <silent> [SPC]sgj :call SpaceVim#plugins#searcher#find("", "grep")
nnoremap <silent> [SPC]sgF :call SpaceVim#mapping#search#grep("g", "F")
nnoremap <silent> [SPC]sgf :call SpaceVim#mapping#search#grep("g", "f")
nnoremap <silent> [SPC]sgP :call SpaceVim#mapping#search#grep("g", "P")
nnoremap <silent> [SPC]sgp :call SpaceVim#mapping#search#grep("g", "p")
nnoremap <silent> [SPC]sgD :call SpaceVim#mapping#search#grep("g", "D")
nnoremap <silent> [SPC]sgd :call SpaceVim#mapping#search#grep("g", "d")
nnoremap <silent> [SPC]sgB :call SpaceVim#mapping#search#grep("g", "B")
nnoremap <silent> [SPC]sgb :call SpaceVim#mapping#search#grep("g", "b")
nnoremap <silent> [SPC]saJ :call SpaceVim#plugins#searcher#find(expand("<cword>"), "ag")
nnoremap <silent> [SPC]saj :call SpaceVim#plugins#searcher#find("", "ag")
nnoremap <silent> [SPC]saF :call SpaceVim#mapping#search#grep("a", "F")
nnoremap <silent> [SPC]saf :call SpaceVim#mapping#search#grep("a", "f")
nnoremap <silent> [SPC]saP :call SpaceVim#mapping#search#grep("a", "P")
nnoremap <silent> [SPC]sap :call SpaceVim#mapping#search#grep("a", "p")
nnoremap <silent> [SPC]saD :call SpaceVim#mapping#search#grep("a", "D")
nnoremap <silent> [SPC]sad :call SpaceVim#mapping#search#grep("a", "d")
nnoremap <silent> [SPC]saB :call SpaceVim#mapping#search#grep("a", "B")
nnoremap <silent> [SPC]sab :call SpaceVim#mapping#search#grep("a", "b")
nnoremap <silent> [SPC]sl :call SpaceVim#plugins#searcher#list()
nnoremap <silent> [SPC]sJ :call SpaceVim#plugins#searcher#find(expand("<cword>"),SpaceVim#mapping#search#default_tool()[0])
nnoremap <silent> [SPC]sj :call SpaceVim#plugins#searcher#find("", SpaceVim#mapping#search#default_tool()[0])
nnoremap <silent> [SPC]sP :call SpaceVim#plugins#flygrep#open({'input' : expand("<cword>"), 'dir' : get(b:, "rootDir", getcwd())})
nnoremap <silent> [SPC]sp :call SpaceVim#plugins#flygrep#open({'input' : input("grep pattern:"), 'dir' : get(b:, "rootDir", getcwd())})
nnoremap <silent> [SPC]sF :call SpaceVim#plugins#flygrep#open({"input" : expand("<cword>"), "dir": input("arbitrary dir:", "", "dir")})
nnoremap <silent> [SPC]sf :call SpaceVim#plugins#flygrep#open({"input" : input("grep pattern:"), "dir": input("arbitrary dir:", "", "dir")})
nnoremap <silent> [SPC]sD :call SpaceVim#plugins#flygrep#open({"input" : expand("<cword>"), "dir": fnamemodify(expand("%"), ":p:h")})
nnoremap <silent> [SPC]sd :call SpaceVim#plugins#flygrep#open({"input" : input("grep pattern:"), "dir": fnamemodify(expand("%"), ":p:h")})
nnoremap <silent> [SPC]sB :call SpaceVim#plugins#flygrep#open({"input" : expand("<cword>"), "files": "@buffers"})
nnoremap <silent> [SPC]sb :call SpaceVim#plugins#flygrep#open({"input" : input("grep pattern:"), "files": "@buffers"})
nnoremap <silent> [SPC]sS :call SpaceVim#plugins#flygrep#open({"input" : expand("<cword>"), "files": bufname("%")})
nnoremap <silent> [SPC]ss :call SpaceVim#plugins#flygrep#open({"input" : input("grep pattern:"), "files": bufname("%")})
nnoremap <silent> [SPC]tn :setlocal number!
nnoremap <silent> [SPC]wU :call SpaceVim#plugins#windowsmanager#RedoQuitWin()
nnoremap <silent> [SPC]wu :call SpaceVim#plugins#windowsmanager#UndoQuitWin()
nnoremap <silent> [SPC]wW :ChooseWin
nnoremap <silent> [SPC]ww :wincmd w
nnoremap <silent> [SPC]w= :wincmd =
nnoremap <silent> [SPC]wV :bel vs
nnoremap <silent> [SPC]w3 :silent only | vs | vs | wincmd H
nnoremap <silent> [SPC]w2 :silent only | vs | wincmd w
nnoremap <silent> [SPC]wS :bel split
nnoremap <silent> [SPC]ws :bel split | wincmd w
nnoremap <silent> [SPC]w- :bel split | wincmd w
nnoremap <silent> [SPC]wv :belowright vsplit | wincmd w
nnoremap <silent> [SPC]w/ :belowright vsplit | wincmd w
nnoremap <silent> [SPC]wo :tabnext
nnoremap <silent> [SPC]wM :execute eval("winnr('$')<=2 ? 'wincmd x' : 'ChooseWinSwap'")
nnoremap <silent> [SPC]wL :wincmd L
nnoremap <silent> [SPC]wK :wincmd K
nnoremap <silent> [SPC]wJ :wincmd J
nnoremap <silent> [SPC]wH :wincmd H
nnoremap <silent> [SPC]wl :wincmd l
nnoremap <silent> [SPC]wk :wincmd k
nnoremap <silent> [SPC]wx :wincmd x
nnoremap <silent> [SPC]wj :wincmd j
nnoremap <silent> [SPC]wh :wincmd h
nnoremap <silent> [SPC]wF :tabnew
nnoremap <silent> [SPC]wD :ChooseWin | close | wincmd w
nnoremap <silent> [SPC]wf :setlocal scrollbind!
nnoremap <silent> [SPC]wd :close
nnoremap <silent> [SPC]w	 :wincmd w
nnoremap <silent> [SPC]9 :call SpaceVim#layers#core#statusline#jump(9)
nnoremap <silent> [SPC]8 :call SpaceVim#layers#core#statusline#jump(8)
nnoremap <silent> [SPC]7 :call SpaceVim#layers#core#statusline#jump(7)
nnoremap <silent> [SPC]6 :call SpaceVim#layers#core#statusline#jump(6)
nnoremap <silent> [SPC]5 :call SpaceVim#layers#core#statusline#jump(5)
nnoremap <silent> [SPC]4 :call SpaceVim#layers#core#statusline#jump(4)
nnoremap <silent> [SPC]3 :call SpaceVim#layers#core#statusline#jump(3)
nnoremap <silent> [SPC]2 :call SpaceVim#layers#core#statusline#jump(2)
nnoremap <silent> [SPC]1 :call SpaceVim#layers#core#statusline#jump(1)
vnoremap <nowait> <silent> [SPC] :LeaderGuideVisual ' '
nnoremap <nowait> <silent> [SPC] :LeaderGuide ' '
nnoremap <nowait> <silent> [Z] :LeaderGuide "z"
nnoremap <nowait> <silent> [G] :LeaderGuide "g"
xmap \T <Plug>(table-mode-tableize-delimiter)
xmap \tt <Plug>(table-mode-tableize)
nmap \tt <Plug>(table-mode-tableize)
nnoremap <silent> \tm :call tablemode#Toggle()
map \\ <Plug>(easymotion-prefix)
nmap <silent> \) :call SpaceVim#layers#core#tabline#jump(20)
nmap <silent> \( :call SpaceVim#layers#core#tabline#jump(19)
nmap <silent> \* :call SpaceVim#layers#core#tabline#jump(18)
nmap <silent> \& :call SpaceVim#layers#core#tabline#jump(17)
nmap <silent> \^ :call SpaceVim#layers#core#tabline#jump(16)
nmap <silent> \% :call SpaceVim#layers#core#tabline#jump(15)
nmap <silent> \$ :call SpaceVim#layers#core#tabline#jump(14)
nmap <silent> \# :call SpaceVim#layers#core#tabline#jump(13)
nmap <silent> \@ :call SpaceVim#layers#core#tabline#jump(12)
nmap <silent> \! :call SpaceVim#layers#core#tabline#jump(11)
nmap <silent> \0 :call SpaceVim#layers#core#tabline#jump(10)
nmap <silent> \9 :call SpaceVim#layers#core#tabline#jump(9)
nmap <silent> \8 :call SpaceVim#layers#core#tabline#jump(8)
nmap <silent> \7 :call SpaceVim#layers#core#tabline#jump(7)
nmap <silent> \6 :call SpaceVim#layers#core#tabline#jump(6)
nmap <silent> \5 :call SpaceVim#layers#core#tabline#jump(5)
nmap <silent> \4 :call SpaceVim#layers#core#tabline#jump(4)
nmap <silent> \3 :call SpaceVim#layers#core#tabline#jump(3)
nmap <silent> \2 :call SpaceVim#layers#core#tabline#jump(2)
nmap <silent> \1 :call SpaceVim#layers#core#tabline#jump(1)
vnoremap <silent> \ :LeaderGuideVisual get(g:, 'mapleader', '\')
nnoremap <nowait> <silent> \ :LeaderGuide get(g:, 'mapleader', '\')
nnoremap <silent> \qc :call setqflist([])
nnoremap <silent> \qr q
nnoremap \ql :copen
nnoremap \qp :cprev
nnoremap \qn :cnext
xnoremap <silent> \Y :call SpaceVim#plugins#pastebin#paste()
xnoremap <expr> \P clipboard#paste('P')
xnoremap <expr> \p clipboard#paste('p')
nnoremap <expr> \P clipboard#paste('P')
nnoremap <expr> \p clipboard#paste('p')
xnoremap <silent> \y :call clipboard#yank()
omap <silent> ]% <Plug>(matchup-]%)
xmap <silent> ]% <Plug>(matchup-]%)
nmap <silent> ]% <Plug>(matchup-]%)
nnoremap <silent> ]p p
nnoremap <silent> ]t :tabnext
nnoremap <silent> ]l :lnext
nnoremap <silent> ]b :bn | stopinsert
nnoremap <silent> ]e :execute 'move +'. v:count1
nnoremap <silent> ]  :put =repeat(nr2char(10), v:count1)
vnoremap <silent> ]<Home> dggP``
vnoremap <silent> ]<End> dGp``
nnoremap <silent> ]<Home> ddggP``
nnoremap <silent> ]<End> ddGp``
omap ae <Plug>(textobj-entire-a)
xmap ae <Plug>(textobj-entire-a)
omap <silent> a% <Plug>(matchup-a%)
xmap <silent> a% <Plug>(matchup-a%)
omap ai <Plug>(textobj-indent-a)
xmap ai <Plug>(textobj-indent-a)
omap aI <Plug>(textobj-indent-same-a)
xmap aI <Plug>(textobj-indent-same-a)
omap al <Plug>(textobj-line-a)
xmap al <Plug>(textobj-line-a)
nmap cS <Plug>CSurround
nmap cs <Plug>Csurround
nmap ds <Plug>Dsurround
nmap <silent> d<Plug>(easymotion-prefix)l <Plug>(easyoperator-line-delete)
xmap gS <Plug>VgSurround
omap <silent> g% <Plug>(matchup-g%)
xmap <silent> g% <Plug>(matchup-g%)
nmap <silent> g% <Plug>(matchup-g%)
vmap gx <Plug>(openbrowser-smart-search)
nmap gx <Plug>(openbrowser-smart-search)
vmap gs <Plug>(openbrowser-search)
vmap go <Plug>(openbrowser-open)
nnoremap <silent> <expr> gp '`['.strpart(getregtype(), 0, 1).'`]'
nnoremap <silent> g= :call SpaceVim#mapping#format()
nnoremap <silent> gd :call SpaceVim#mapping#gd()
nnoremap <silent> gD :call SpaceVim#mapping#g_capital_d()
nnoremap g<C-]> g
nnoremap g g
nnoremap gv gv
nmap gs <Plug>(openbrowser-search)
nmap go <Plug>(openbrowser-open)
nnoremap gm gm
nnoremap gg gg
nnoremap ga ga
nnoremap g~ g~
nnoremap g_ g_
nnoremap g^ g^
nnoremap g] g]
nnoremap gt gt
nnoremap gT gT
nnoremap gR gR
nnoremap gq gq
nnoremap gQ gQ
nnoremap gn gn
nnoremap gN gN
nnoremap gJ gJ
nnoremap gi gi
nnoremap gI gI
nnoremap gh gh
nnoremap gH gH
nnoremap gU gU
nnoremap gE gE
nnoremap gu gu
nnoremap gk gk
nnoremap gj gj
nnoremap gF gF
nnoremap gf gf
nnoremap g< g<
nnoremap ge ge
nnoremap g<Home> g<Home>
nnoremap <silent> g0 :tabfirst
nnoremap g<End> g<End>
nnoremap <silent> g$ :tablast
nnoremap g@ g@
nnoremap g; g;
nnoremap g, g,
nnoremap g- g-
nnoremap g+ g+
nnoremap g` g`
nnoremap g' g'
nnoremap g& g&
nnoremap g<C-G> g
nnoremap g g
nmap g [G]
nnoremap <silent> <expr> gr tabpagenr('#') > 0 ? ':exe "tabnext " . tabpagenr("#")' : ''
omap ie <Plug>(textobj-entire-i)
xmap ie <Plug>(textobj-entire-i)
omap <silent> i% <Plug>(matchup-i%)
xmap <silent> i% <Plug>(matchup-i%)
omap ii <Plug>(textobj-indent-i)
xmap ii <Plug>(textobj-indent-i)
omap iI <Plug>(textobj-indent-same-i)
xmap iI <Plug>(textobj-indent-same-i)
omap il <Plug>(textobj-line-i)
xmap il <Plug>(textobj-line-i)
xmap v <Plug>(expand_region_expand)
nmap ySS <Plug>YSsurround
nmap ySs <Plug>YSsurround
nmap yss <Plug>Yssurround
nmap yS <Plug>YSurround
nmap ys <Plug>Ysurround
nmap <silent> y<Plug>(easymotion-prefix)l <Plug>(easyoperator-line-yank)
omap <silent> z% <Plug>(matchup-z%)
xmap <silent> z% <Plug>(matchup-z%)
nmap <silent> z% <Plug>(matchup-z%)
nnoremap zx zx
nnoremap zw zw
nnoremap zv zv
nnoremap zt zt
nnoremap zs zs
nnoremap zr zr
nnoremap zo zo
nnoremap zn zn
nnoremap zm zm
nnoremap z<Right> zl
nnoremap zl zl
nnoremap zK zkzx
nnoremap zk zk
nnoremap zJ zjzx
nnoremap zj zj
nnoremap zi zi
nnoremap z<Left> zh
nnoremap zh zh
nnoremap zg zg
nnoremap zf zf
nnoremap ze ze
nnoremap zd zd
nnoremap zc zc
nnoremap zb zb
nnoremap za za
nnoremap zX zX
nnoremap zW zW
nnoremap zR zR
nnoremap zO zO
nnoremap zN zN
nnoremap zM zM
nnoremap zL zL
nnoremap zH zH
nnoremap zG zG
nnoremap zF zF
nnoremap zE zE
nnoremap zD zD
nnoremap zC zC
nnoremap zA zA
nnoremap z= z=
nnoremap z. z.
nnoremap z^ z^
nnoremap z- z-
nnoremap z+ z+
nnoremap z z
nmap z [Z]
nnoremap zz zz
nnoremap <silent> <Plug>(nohlsearch) :nohlsearch
xnoremap <silent> <Plug>NERDCommenterUncomment :call NERDComment("x", "Uncomment")
nnoremap <silent> <Plug>NERDCommenterUncomment :call NERDComment("n", "Uncomment")
xnoremap <silent> <Plug>NERDCommenterAlignBoth :call NERDComment("x", "AlignBoth")
nnoremap <silent> <Plug>NERDCommenterAlignBoth :call NERDComment("n", "AlignBoth")
xnoremap <silent> <Plug>NERDCommenterAlignLeft :call NERDComment("x", "AlignLeft")
nnoremap <silent> <Plug>NERDCommenterAlignLeft :call NERDComment("n", "AlignLeft")
nnoremap <silent> <Plug>NERDCommenterAppend :call NERDComment("n", "Append")
xnoremap <silent> <Plug>NERDCommenterYank :call NERDComment("x", "Yank")
nnoremap <silent> <Plug>NERDCommenterYank :call NERDComment("n", "Yank")
xnoremap <silent> <Plug>NERDCommenterSexy :call NERDComment("x", "Sexy")
nnoremap <silent> <Plug>NERDCommenterSexy :call NERDComment("n", "Sexy")
xnoremap <silent> <Plug>NERDCommenterInvert :call NERDComment("x", "Invert")
nnoremap <silent> <Plug>NERDCommenterInvert :call NERDComment("n", "Invert")
nnoremap <silent> <Plug>NERDCommenterToEOL :call NERDComment("n", "ToEOL")
xnoremap <silent> <Plug>NERDCommenterNested :call NERDComment("x", "Nested")
nnoremap <silent> <Plug>NERDCommenterNested :call NERDComment("n", "Nested")
xnoremap <silent> <Plug>NERDCommenterMinimal :call NERDComment("x", "Minimal")
nnoremap <silent> <Plug>NERDCommenterMinimal :call NERDComment("n", "Minimal")
xnoremap <silent> <Plug>NERDCommenterToggle :call NERDComment("x", "Toggle")
nnoremap <silent> <Plug>NERDCommenterToggle :call NERDComment("n", "Toggle")
xnoremap <silent> <Plug>NERDCommenterComment :call NERDComment("x", "Comment")
nnoremap <silent> <Plug>NERDCommenterComment :call NERDComment("n", "Comment")
nnoremap <silent> <Plug>SurroundRepeat .
nnoremap <silent> <Plug>(table-mode-sort) :call tablemode#spreadsheet#Sort('')
nnoremap <silent> <Plug>(table-mode-eval-formula) :call tablemode#spreadsheet#formula#EvaluateFormulaLine()
nnoremap <silent> <Plug>(table-mode-add-formula) :call tablemode#spreadsheet#formula#Add()
nnoremap <silent> <Plug>(table-mode-insert-column-after) :call tablemode#spreadsheet#InsertColumn(1)
nnoremap <silent> <Plug>(table-mode-insert-column-before) :call tablemode#spreadsheet#InsertColumn(0)
nnoremap <silent> <Plug>(table-mode-delete-column) :call tablemode#spreadsheet#DeleteColumn()
nnoremap <silent> <Plug>(table-mode-delete-row) :call tablemode#spreadsheet#DeleteRow()
xnoremap <silent> <Plug>(table-mode-cell-text-object-i) :call tablemode#spreadsheet#cell#TextObject(1)
xnoremap <silent> <Plug>(table-mode-cell-text-object-a) :call tablemode#spreadsheet#cell#TextObject(0)
onoremap <silent> <Plug>(table-mode-cell-text-object-i) :call tablemode#spreadsheet#cell#TextObject(1)
onoremap <silent> <Plug>(table-mode-cell-text-object-a) :call tablemode#spreadsheet#cell#TextObject(0)
nnoremap <silent> <Plug>(table-mode-motion-right) :call tablemode#spreadsheet#cell#Motion('l')
nnoremap <silent> <Plug>(table-mode-motion-left) :call tablemode#spreadsheet#cell#Motion('h')
nnoremap <silent> <Plug>(table-mode-motion-down) :call tablemode#spreadsheet#cell#Motion('j')
nnoremap <silent> <Plug>(table-mode-motion-up) :call tablemode#spreadsheet#cell#Motion('k')
nnoremap <silent> <Plug>(table-mode-realign) :call tablemode#table#Realign('.')
xnoremap <silent> <Plug>(table-mode-tableize-delimiter) :call tablemode#TableizeByDelimiter()
xnoremap <silent> <Plug>(table-mode-tableize) :Tableize
nnoremap <silent> <Plug>(table-mode-tableize) :Tableize
nnoremap <silent> <Plug>(startify-open-buffers) :call startify#open_buffers()
map <silent> <Plug>(easymotion-prefix)N <Plug>(easymotion-N)
map <silent> <Plug>(easymotion-prefix)n <Plug>(easymotion-n)
map <silent> <Plug>(easymotion-prefix)k <Plug>(easymotion-k)
map <silent> <Plug>(easymotion-prefix)j <Plug>(easymotion-j)
map <silent> <Plug>(easymotion-prefix)gE <Plug>(easymotion-gE)
map <silent> <Plug>(easymotion-prefix)ge <Plug>(easymotion-ge)
map <silent> <Plug>(easymotion-prefix)E <Plug>(easymotion-E)
map <silent> <Plug>(easymotion-prefix)e <Plug>(easymotion-e)
map <silent> <Plug>(easymotion-prefix)B <Plug>(easymotion-B)
map <silent> <Plug>(easymotion-prefix)b <Plug>(easymotion-b)
map <silent> <Plug>(easymotion-prefix)W <Plug>(easymotion-W)
map <silent> <Plug>(easymotion-prefix)w <Plug>(easymotion-w)
map <silent> <Plug>(easymotion-prefix)T <Plug>(easymotion-T)
map <silent> <Plug>(easymotion-prefix)t <Plug>(easymotion-t)
map <silent> <Plug>(easymotion-prefix)s <Plug>(easymotion-s)
map <silent> <Plug>(easymotion-prefix)F <Plug>(easymotion-F)
map <silent> <Plug>(easymotion-prefix)f <Plug>(easymotion-f)
xnoremap <silent> <Plug>(easymotion-activate) :call EasyMotion#activate(1)
nnoremap <silent> <Plug>(easymotion-activate) :call EasyMotion#activate(0)
snoremap <silent> <Plug>(easymotion-activate) :call EasyMotion#activate(0)
onoremap <silent> <Plug>(easymotion-activate) :call EasyMotion#activate(0)
noremap <silent> <Plug>(easymotion-dotrepeat) :call EasyMotion#DotRepeat()
xnoremap <silent> <Plug>(easymotion-repeat) :call EasyMotion#Repeat(1)
nnoremap <silent> <Plug>(easymotion-repeat) :call EasyMotion#Repeat(0)
snoremap <silent> <Plug>(easymotion-repeat) :call EasyMotion#Repeat(0)
onoremap <silent> <Plug>(easymotion-repeat) :call EasyMotion#Repeat(0)
xnoremap <silent> <Plug>(easymotion-prev) :call EasyMotion#NextPrevious(1,1)
nnoremap <silent> <Plug>(easymotion-prev) :call EasyMotion#NextPrevious(0,1)
snoremap <silent> <Plug>(easymotion-prev) :call EasyMotion#NextPrevious(0,1)
onoremap <silent> <Plug>(easymotion-prev) :call EasyMotion#NextPrevious(0,1)
xnoremap <silent> <Plug>(easymotion-next) :call EasyMotion#NextPrevious(1,0)
nnoremap <silent> <Plug>(easymotion-next) :call EasyMotion#NextPrevious(0,0)
snoremap <silent> <Plug>(easymotion-next) :call EasyMotion#NextPrevious(0,0)
onoremap <silent> <Plug>(easymotion-next) :call EasyMotion#NextPrevious(0,0)
xnoremap <silent> <Plug>(easymotion-wl) :call EasyMotion#WBL(1,0)
nnoremap <silent> <Plug>(easymotion-wl) :call EasyMotion#WBL(0,0)
snoremap <silent> <Plug>(easymotion-wl) :call EasyMotion#WBL(0,0)
onoremap <silent> <Plug>(easymotion-wl) :call EasyMotion#WBL(0,0)
xnoremap <silent> <Plug>(easymotion-lineforward) :call EasyMotion#LineAnywhere(1,0)
nnoremap <silent> <Plug>(easymotion-lineforward) :call EasyMotion#LineAnywhere(0,0)
snoremap <silent> <Plug>(easymotion-lineforward) :call EasyMotion#LineAnywhere(0,0)
onoremap <silent> <Plug>(easymotion-lineforward) :call EasyMotion#LineAnywhere(0,0)
xnoremap <silent> <Plug>(easymotion-lineanywhere) :call EasyMotion#LineAnywhere(1,2)
nnoremap <silent> <Plug>(easymotion-lineanywhere) :call EasyMotion#LineAnywhere(0,2)
snoremap <silent> <Plug>(easymotion-lineanywhere) :call EasyMotion#LineAnywhere(0,2)
onoremap <silent> <Plug>(easymotion-lineanywhere) :call EasyMotion#LineAnywhere(0,2)
xnoremap <silent> <Plug>(easymotion-bd-wl) :call EasyMotion#WBL(1,2)
nnoremap <silent> <Plug>(easymotion-bd-wl) :call EasyMotion#WBL(0,2)
snoremap <silent> <Plug>(easymotion-bd-wl) :call EasyMotion#WBL(0,2)
onoremap <silent> <Plug>(easymotion-bd-wl) :call EasyMotion#WBL(0,2)
xnoremap <silent> <Plug>(easymotion-linebackward) :call EasyMotion#LineAnywhere(1,1)
nnoremap <silent> <Plug>(easymotion-linebackward) :call EasyMotion#LineAnywhere(0,1)
snoremap <silent> <Plug>(easymotion-linebackward) :call EasyMotion#LineAnywhere(0,1)
onoremap <silent> <Plug>(easymotion-linebackward) :call EasyMotion#LineAnywhere(0,1)
xnoremap <silent> <Plug>(easymotion-bl) :call EasyMotion#WBL(1,1)
nnoremap <silent> <Plug>(easymotion-bl) :call EasyMotion#WBL(0,1)
snoremap <silent> <Plug>(easymotion-bl) :call EasyMotion#WBL(0,1)
onoremap <silent> <Plug>(easymotion-bl) :call EasyMotion#WBL(0,1)
xnoremap <silent> <Plug>(easymotion-el) :call EasyMotion#EL(1,0)
nnoremap <silent> <Plug>(easymotion-el) :call EasyMotion#EL(0,0)
snoremap <silent> <Plug>(easymotion-el) :call EasyMotion#EL(0,0)
onoremap <silent> <Plug>(easymotion-el) :call EasyMotion#EL(0,0)
xnoremap <silent> <Plug>(easymotion-gel) :call EasyMotion#EL(1,1)
nnoremap <silent> <Plug>(easymotion-gel) :call EasyMotion#EL(0,1)
snoremap <silent> <Plug>(easymotion-gel) :call EasyMotion#EL(0,1)
onoremap <silent> <Plug>(easymotion-gel) :call EasyMotion#EL(0,1)
xnoremap <silent> <Plug>(easymotion-bd-el) :call EasyMotion#EL(1,2)
nnoremap <silent> <Plug>(easymotion-bd-el) :call EasyMotion#EL(0,2)
snoremap <silent> <Plug>(easymotion-bd-el) :call EasyMotion#EL(0,2)
onoremap <silent> <Plug>(easymotion-bd-el) :call EasyMotion#EL(0,2)
xnoremap <silent> <Plug>(easymotion-jumptoanywhere) :call EasyMotion#JumpToAnywhere(1,2)
nnoremap <silent> <Plug>(easymotion-jumptoanywhere) :call EasyMotion#JumpToAnywhere(0,2)
snoremap <silent> <Plug>(easymotion-jumptoanywhere) :call EasyMotion#JumpToAnywhere(0,2)
onoremap <silent> <Plug>(easymotion-jumptoanywhere) :call EasyMotion#JumpToAnywhere(0,2)
xnoremap <silent> <Plug>(easymotion-vim-n) :call EasyMotion#Search(1,0,1)
nnoremap <silent> <Plug>(easymotion-vim-n) :call EasyMotion#Search(0,0,1)
snoremap <silent> <Plug>(easymotion-vim-n) :call EasyMotion#Search(0,0,1)
onoremap <silent> <Plug>(easymotion-vim-n) :call EasyMotion#Search(0,0,1)
xnoremap <silent> <Plug>(easymotion-n) :call EasyMotion#Search(1,0,0)
nnoremap <silent> <Plug>(easymotion-n) :call EasyMotion#Search(0,0,0)
snoremap <silent> <Plug>(easymotion-n) :call EasyMotion#Search(0,0,0)
onoremap <silent> <Plug>(easymotion-n) :call EasyMotion#Search(0,0,0)
xnoremap <silent> <Plug>(easymotion-bd-n) :call EasyMotion#Search(1,2,0)
nnoremap <silent> <Plug>(easymotion-bd-n) :call EasyMotion#Search(0,2,0)
snoremap <silent> <Plug>(easymotion-bd-n) :call EasyMotion#Search(0,2,0)
onoremap <silent> <Plug>(easymotion-bd-n) :call EasyMotion#Search(0,2,0)
xnoremap <silent> <Plug>(easymotion-vim-N) :call EasyMotion#Search(1,1,1)
nnoremap <silent> <Plug>(easymotion-vim-N) :call EasyMotion#Search(0,1,1)
snoremap <silent> <Plug>(easymotion-vim-N) :call EasyMotion#Search(0,1,1)
onoremap <silent> <Plug>(easymotion-vim-N) :call EasyMotion#Search(0,1,1)
xnoremap <silent> <Plug>(easymotion-N) :call EasyMotion#Search(1,1,0)
nnoremap <silent> <Plug>(easymotion-N) :call EasyMotion#Search(0,1,0)
snoremap <silent> <Plug>(easymotion-N) :call EasyMotion#Search(0,1,0)
onoremap <silent> <Plug>(easymotion-N) :call EasyMotion#Search(0,1,0)
xnoremap <silent> <Plug>(easymotion-eol-j) :call EasyMotion#Eol(1,0)
nnoremap <silent> <Plug>(easymotion-eol-j) :call EasyMotion#Eol(0,0)
snoremap <silent> <Plug>(easymotion-eol-j) :call EasyMotion#Eol(0,0)
onoremap <silent> <Plug>(easymotion-eol-j) :call EasyMotion#Eol(0,0)
xnoremap <silent> <Plug>(easymotion-sol-k) :call EasyMotion#Sol(1,1)
nnoremap <silent> <Plug>(easymotion-sol-k) :call EasyMotion#Sol(0,1)
snoremap <silent> <Plug>(easymotion-sol-k) :call EasyMotion#Sol(0,1)
onoremap <silent> <Plug>(easymotion-sol-k) :call EasyMotion#Sol(0,1)
xnoremap <silent> <Plug>(easymotion-sol-j) :call EasyMotion#Sol(1,0)
nnoremap <silent> <Plug>(easymotion-sol-j) :call EasyMotion#Sol(0,0)
snoremap <silent> <Plug>(easymotion-sol-j) :call EasyMotion#Sol(0,0)
onoremap <silent> <Plug>(easymotion-sol-j) :call EasyMotion#Sol(0,0)
xnoremap <silent> <Plug>(easymotion-k) :call EasyMotion#JK(1,1)
nnoremap <silent> <Plug>(easymotion-k) :call EasyMotion#JK(0,1)
snoremap <silent> <Plug>(easymotion-k) :call EasyMotion#JK(0,1)
onoremap <silent> <Plug>(easymotion-k) :call EasyMotion#JK(0,1)
xnoremap <silent> <Plug>(easymotion-j) :call EasyMotion#JK(1,0)
nnoremap <silent> <Plug>(easymotion-j) :call EasyMotion#JK(0,0)
snoremap <silent> <Plug>(easymotion-j) :call EasyMotion#JK(0,0)
onoremap <silent> <Plug>(easymotion-j) :call EasyMotion#JK(0,0)
xnoremap <silent> <Plug>(easymotion-bd-jk) :call EasyMotion#JK(1,2)
nnoremap <silent> <Plug>(easymotion-bd-jk) :call EasyMotion#JK(0,2)
snoremap <silent> <Plug>(easymotion-bd-jk) :call EasyMotion#JK(0,2)
onoremap <silent> <Plug>(easymotion-bd-jk) :call EasyMotion#JK(0,2)
xnoremap <silent> <Plug>(easymotion-eol-bd-jk) :call EasyMotion#Eol(1,2)
nnoremap <silent> <Plug>(easymotion-eol-bd-jk) :call EasyMotion#Eol(0,2)
snoremap <silent> <Plug>(easymotion-eol-bd-jk) :call EasyMotion#Eol(0,2)
onoremap <silent> <Plug>(easymotion-eol-bd-jk) :call EasyMotion#Eol(0,2)
xnoremap <silent> <Plug>(easymotion-sol-bd-jk) :call EasyMotion#Sol(1,2)
nnoremap <silent> <Plug>(easymotion-sol-bd-jk) :call EasyMotion#Sol(0,2)
snoremap <silent> <Plug>(easymotion-sol-bd-jk) :call EasyMotion#Sol(0,2)
onoremap <silent> <Plug>(easymotion-sol-bd-jk) :call EasyMotion#Sol(0,2)
xnoremap <silent> <Plug>(easymotion-eol-k) :call EasyMotion#Eol(1,1)
nnoremap <silent> <Plug>(easymotion-eol-k) :call EasyMotion#Eol(0,1)
snoremap <silent> <Plug>(easymotion-eol-k) :call EasyMotion#Eol(0,1)
onoremap <silent> <Plug>(easymotion-eol-k) :call EasyMotion#Eol(0,1)
xnoremap <silent> <Plug>(easymotion-iskeyword-ge) :call EasyMotion#EK(1,1)
nnoremap <silent> <Plug>(easymotion-iskeyword-ge) :call EasyMotion#EK(0,1)
snoremap <silent> <Plug>(easymotion-iskeyword-ge) :call EasyMotion#EK(0,1)
onoremap <silent> <Plug>(easymotion-iskeyword-ge) :call EasyMotion#EK(0,1)
xnoremap <silent> <Plug>(easymotion-w) :call EasyMotion#WB(1,0)
nnoremap <silent> <Plug>(easymotion-w) :call EasyMotion#WB(0,0)
snoremap <silent> <Plug>(easymotion-w) :call EasyMotion#WB(0,0)
onoremap <silent> <Plug>(easymotion-w) :call EasyMotion#WB(0,0)
xnoremap <silent> <Plug>(easymotion-bd-W) :call EasyMotion#WBW(1,2)
nnoremap <silent> <Plug>(easymotion-bd-W) :call EasyMotion#WBW(0,2)
snoremap <silent> <Plug>(easymotion-bd-W) :call EasyMotion#WBW(0,2)
onoremap <silent> <Plug>(easymotion-bd-W) :call EasyMotion#WBW(0,2)
xnoremap <silent> <Plug>(easymotion-iskeyword-w) :call EasyMotion#WBK(1,0)
nnoremap <silent> <Plug>(easymotion-iskeyword-w) :call EasyMotion#WBK(0,0)
snoremap <silent> <Plug>(easymotion-iskeyword-w) :call EasyMotion#WBK(0,0)
onoremap <silent> <Plug>(easymotion-iskeyword-w) :call EasyMotion#WBK(0,0)
xnoremap <silent> <Plug>(easymotion-gE) :call EasyMotion#EW(1,1)
nnoremap <silent> <Plug>(easymotion-gE) :call EasyMotion#EW(0,1)
snoremap <silent> <Plug>(easymotion-gE) :call EasyMotion#EW(0,1)
onoremap <silent> <Plug>(easymotion-gE) :call EasyMotion#EW(0,1)
xnoremap <silent> <Plug>(easymotion-e) :call EasyMotion#E(1,0)
nnoremap <silent> <Plug>(easymotion-e) :call EasyMotion#E(0,0)
snoremap <silent> <Plug>(easymotion-e) :call EasyMotion#E(0,0)
onoremap <silent> <Plug>(easymotion-e) :call EasyMotion#E(0,0)
xnoremap <silent> <Plug>(easymotion-bd-E) :call EasyMotion#EW(1,2)
nnoremap <silent> <Plug>(easymotion-bd-E) :call EasyMotion#EW(0,2)
snoremap <silent> <Plug>(easymotion-bd-E) :call EasyMotion#EW(0,2)
onoremap <silent> <Plug>(easymotion-bd-E) :call EasyMotion#EW(0,2)
xnoremap <silent> <Plug>(easymotion-iskeyword-e) :call EasyMotion#EK(1,0)
nnoremap <silent> <Plug>(easymotion-iskeyword-e) :call EasyMotion#EK(0,0)
snoremap <silent> <Plug>(easymotion-iskeyword-e) :call EasyMotion#EK(0,0)
onoremap <silent> <Plug>(easymotion-iskeyword-e) :call EasyMotion#EK(0,0)
xnoremap <silent> <Plug>(easymotion-b) :call EasyMotion#WB(1,1)
nnoremap <silent> <Plug>(easymotion-b) :call EasyMotion#WB(0,1)
snoremap <silent> <Plug>(easymotion-b) :call EasyMotion#WB(0,1)
onoremap <silent> <Plug>(easymotion-b) :call EasyMotion#WB(0,1)
xnoremap <silent> <Plug>(easymotion-iskeyword-b) :call EasyMotion#WBK(1,1)
nnoremap <silent> <Plug>(easymotion-iskeyword-b) :call EasyMotion#WBK(0,1)
snoremap <silent> <Plug>(easymotion-iskeyword-b) :call EasyMotion#WBK(0,1)
onoremap <silent> <Plug>(easymotion-iskeyword-b) :call EasyMotion#WBK(0,1)
xnoremap <silent> <Plug>(easymotion-iskeyword-bd-w) :call EasyMotion#WBK(1,2)
nnoremap <silent> <Plug>(easymotion-iskeyword-bd-w) :call EasyMotion#WBK(0,2)
snoremap <silent> <Plug>(easymotion-iskeyword-bd-w) :call EasyMotion#WBK(0,2)
onoremap <silent> <Plug>(easymotion-iskeyword-bd-w) :call EasyMotion#WBK(0,2)
xnoremap <silent> <Plug>(easymotion-W) :call EasyMotion#WBW(1,0)
nnoremap <silent> <Plug>(easymotion-W) :call EasyMotion#WBW(0,0)
snoremap <silent> <Plug>(easymotion-W) :call EasyMotion#WBW(0,0)
onoremap <silent> <Plug>(easymotion-W) :call EasyMotion#WBW(0,0)
xnoremap <silent> <Plug>(easymotion-bd-w) :call EasyMotion#WB(1,2)
nnoremap <silent> <Plug>(easymotion-bd-w) :call EasyMotion#WB(0,2)
snoremap <silent> <Plug>(easymotion-bd-w) :call EasyMotion#WB(0,2)
onoremap <silent> <Plug>(easymotion-bd-w) :call EasyMotion#WB(0,2)
xnoremap <silent> <Plug>(easymotion-iskeyword-bd-e) :call EasyMotion#EK(1,2)
nnoremap <silent> <Plug>(easymotion-iskeyword-bd-e) :call EasyMotion#EK(0,2)
snoremap <silent> <Plug>(easymotion-iskeyword-bd-e) :call EasyMotion#EK(0,2)
onoremap <silent> <Plug>(easymotion-iskeyword-bd-e) :call EasyMotion#EK(0,2)
xnoremap <silent> <Plug>(easymotion-ge) :call EasyMotion#E(1,1)
nnoremap <silent> <Plug>(easymotion-ge) :call EasyMotion#E(0,1)
snoremap <silent> <Plug>(easymotion-ge) :call EasyMotion#E(0,1)
onoremap <silent> <Plug>(easymotion-ge) :call EasyMotion#E(0,1)
xnoremap <silent> <Plug>(easymotion-E) :call EasyMotion#EW(1,0)
nnoremap <silent> <Plug>(easymotion-E) :call EasyMotion#EW(0,0)
snoremap <silent> <Plug>(easymotion-E) :call EasyMotion#EW(0,0)
onoremap <silent> <Plug>(easymotion-E) :call EasyMotion#EW(0,0)
xnoremap <silent> <Plug>(easymotion-bd-e) :call EasyMotion#E(1,2)
nnoremap <silent> <Plug>(easymotion-bd-e) :call EasyMotion#E(0,2)
snoremap <silent> <Plug>(easymotion-bd-e) :call EasyMotion#E(0,2)
onoremap <silent> <Plug>(easymotion-bd-e) :call EasyMotion#E(0,2)
xnoremap <silent> <Plug>(easymotion-B) :call EasyMotion#WBW(1,1)
nnoremap <silent> <Plug>(easymotion-B) :call EasyMotion#WBW(0,1)
snoremap <silent> <Plug>(easymotion-B) :call EasyMotion#WBW(0,1)
onoremap <silent> <Plug>(easymotion-B) :call EasyMotion#WBW(0,1)
nnoremap <silent> <Plug>(easymotion-overwin-w) :call EasyMotion#overwin#w()
nnoremap <silent> <Plug>(easymotion-overwin-line) :call EasyMotion#overwin#line()
nnoremap <silent> <Plug>(easymotion-overwin-f2) :call EasyMotion#OverwinF(2)
nnoremap <silent> <Plug>(easymotion-overwin-f) :call EasyMotion#OverwinF(1)
xnoremap <silent> <Plug>(easymotion-Tln) :call EasyMotion#TL(-1,1,1)
nnoremap <silent> <Plug>(easymotion-Tln) :call EasyMotion#TL(-1,0,1)
snoremap <silent> <Plug>(easymotion-Tln) :call EasyMotion#TL(-1,0,1)
onoremap <silent> <Plug>(easymotion-Tln) :call EasyMotion#TL(-1,0,1)
xnoremap <silent> <Plug>(easymotion-t2) :call EasyMotion#T(2,1,0)
nnoremap <silent> <Plug>(easymotion-t2) :call EasyMotion#T(2,0,0)
snoremap <silent> <Plug>(easymotion-t2) :call EasyMotion#T(2,0,0)
onoremap <silent> <Plug>(easymotion-t2) :call EasyMotion#T(2,0,0)
xnoremap <silent> <Plug>(easymotion-t) :call EasyMotion#T(1,1,0)
nnoremap <silent> <Plug>(easymotion-t) :call EasyMotion#T(1,0,0)
snoremap <silent> <Plug>(easymotion-t) :call EasyMotion#T(1,0,0)
onoremap <silent> <Plug>(easymotion-t) :call EasyMotion#T(1,0,0)
xnoremap <silent> <Plug>(easymotion-s) :call EasyMotion#S(1,1,2)
nnoremap <silent> <Plug>(easymotion-s) :call EasyMotion#S(1,0,2)
snoremap <silent> <Plug>(easymotion-s) :call EasyMotion#S(1,0,2)
onoremap <silent> <Plug>(easymotion-s) :call EasyMotion#S(1,0,2)
xnoremap <silent> <Plug>(easymotion-tn) :call EasyMotion#T(-1,1,0)
nnoremap <silent> <Plug>(easymotion-tn) :call EasyMotion#T(-1,0,0)
snoremap <silent> <Plug>(easymotion-tn) :call EasyMotion#T(-1,0,0)
onoremap <silent> <Plug>(easymotion-tn) :call EasyMotion#T(-1,0,0)
xnoremap <silent> <Plug>(easymotion-bd-t2) :call EasyMotion#T(2,1,2)
nnoremap <silent> <Plug>(easymotion-bd-t2) :call EasyMotion#T(2,0,2)
snoremap <silent> <Plug>(easymotion-bd-t2) :call EasyMotion#T(2,0,2)
onoremap <silent> <Plug>(easymotion-bd-t2) :call EasyMotion#T(2,0,2)
xnoremap <silent> <Plug>(easymotion-tl) :call EasyMotion#TL(1,1,0)
nnoremap <silent> <Plug>(easymotion-tl) :call EasyMotion#TL(1,0,0)
snoremap <silent> <Plug>(easymotion-tl) :call EasyMotion#TL(1,0,0)
onoremap <silent> <Plug>(easymotion-tl) :call EasyMotion#TL(1,0,0)
xnoremap <silent> <Plug>(easymotion-bd-tn) :call EasyMotion#T(-1,1,2)
nnoremap <silent> <Plug>(easymotion-bd-tn) :call EasyMotion#T(-1,0,2)
snoremap <silent> <Plug>(easymotion-bd-tn) :call EasyMotion#T(-1,0,2)
onoremap <silent> <Plug>(easymotion-bd-tn) :call EasyMotion#T(-1,0,2)
xnoremap <silent> <Plug>(easymotion-fn) :call EasyMotion#S(-1,1,0)
nnoremap <silent> <Plug>(easymotion-fn) :call EasyMotion#S(-1,0,0)
snoremap <silent> <Plug>(easymotion-fn) :call EasyMotion#S(-1,0,0)
onoremap <silent> <Plug>(easymotion-fn) :call EasyMotion#S(-1,0,0)
xnoremap <silent> <Plug>(easymotion-bd-tl) :call EasyMotion#TL(1,1,2)
nnoremap <silent> <Plug>(easymotion-bd-tl) :call EasyMotion#TL(1,0,2)
snoremap <silent> <Plug>(easymotion-bd-tl) :call EasyMotion#TL(1,0,2)
onoremap <silent> <Plug>(easymotion-bd-tl) :call EasyMotion#TL(1,0,2)
xnoremap <silent> <Plug>(easymotion-fl) :call EasyMotion#SL(1,1,0)
nnoremap <silent> <Plug>(easymotion-fl) :call EasyMotion#SL(1,0,0)
snoremap <silent> <Plug>(easymotion-fl) :call EasyMotion#SL(1,0,0)
onoremap <silent> <Plug>(easymotion-fl) :call EasyMotion#SL(1,0,0)
xnoremap <silent> <Plug>(easymotion-bd-tl2) :call EasyMotion#TL(2,1,2)
nnoremap <silent> <Plug>(easymotion-bd-tl2) :call EasyMotion#TL(2,0,2)
snoremap <silent> <Plug>(easymotion-bd-tl2) :call EasyMotion#TL(2,0,2)
onoremap <silent> <Plug>(easymotion-bd-tl2) :call EasyMotion#TL(2,0,2)
xnoremap <silent> <Plug>(easymotion-bd-fn) :call EasyMotion#S(-1,1,2)
nnoremap <silent> <Plug>(easymotion-bd-fn) :call EasyMotion#S(-1,0,2)
snoremap <silent> <Plug>(easymotion-bd-fn) :call EasyMotion#S(-1,0,2)
onoremap <silent> <Plug>(easymotion-bd-fn) :call EasyMotion#S(-1,0,2)
xnoremap <silent> <Plug>(easymotion-f) :call EasyMotion#S(1,1,0)
nnoremap <silent> <Plug>(easymotion-f) :call EasyMotion#S(1,0,0)
snoremap <silent> <Plug>(easymotion-f) :call EasyMotion#S(1,0,0)
onoremap <silent> <Plug>(easymotion-f) :call EasyMotion#S(1,0,0)
xnoremap <silent> <Plug>(easymotion-bd-fl) :call EasyMotion#SL(1,1,2)
nnoremap <silent> <Plug>(easymotion-bd-fl) :call EasyMotion#SL(1,0,2)
snoremap <silent> <Plug>(easymotion-bd-fl) :call EasyMotion#SL(1,0,2)
onoremap <silent> <Plug>(easymotion-bd-fl) :call EasyMotion#SL(1,0,2)
xnoremap <silent> <Plug>(easymotion-Fl2) :call EasyMotion#SL(2,1,1)
nnoremap <silent> <Plug>(easymotion-Fl2) :call EasyMotion#SL(2,0,1)
snoremap <silent> <Plug>(easymotion-Fl2) :call EasyMotion#SL(2,0,1)
onoremap <silent> <Plug>(easymotion-Fl2) :call EasyMotion#SL(2,0,1)
xnoremap <silent> <Plug>(easymotion-tl2) :call EasyMotion#TL(2,1,0)
nnoremap <silent> <Plug>(easymotion-tl2) :call EasyMotion#TL(2,0,0)
snoremap <silent> <Plug>(easymotion-tl2) :call EasyMotion#TL(2,0,0)
onoremap <silent> <Plug>(easymotion-tl2) :call EasyMotion#TL(2,0,0)
xnoremap <silent> <Plug>(easymotion-f2) :call EasyMotion#S(2,1,0)
nnoremap <silent> <Plug>(easymotion-f2) :call EasyMotion#S(2,0,0)
snoremap <silent> <Plug>(easymotion-f2) :call EasyMotion#S(2,0,0)
onoremap <silent> <Plug>(easymotion-f2) :call EasyMotion#S(2,0,0)
xnoremap <silent> <Plug>(easymotion-Fln) :call EasyMotion#SL(-1,1,1)
nnoremap <silent> <Plug>(easymotion-Fln) :call EasyMotion#SL(-1,0,1)
snoremap <silent> <Plug>(easymotion-Fln) :call EasyMotion#SL(-1,0,1)
onoremap <silent> <Plug>(easymotion-Fln) :call EasyMotion#SL(-1,0,1)
xnoremap <silent> <Plug>(easymotion-sln) :call EasyMotion#SL(-1,1,2)
nnoremap <silent> <Plug>(easymotion-sln) :call EasyMotion#SL(-1,0,2)
snoremap <silent> <Plug>(easymotion-sln) :call EasyMotion#SL(-1,0,2)
onoremap <silent> <Plug>(easymotion-sln) :call EasyMotion#SL(-1,0,2)
xnoremap <silent> <Plug>(easymotion-tln) :call EasyMotion#TL(-1,1,0)
nnoremap <silent> <Plug>(easymotion-tln) :call EasyMotion#TL(-1,0,0)
snoremap <silent> <Plug>(easymotion-tln) :call EasyMotion#TL(-1,0,0)
onoremap <silent> <Plug>(easymotion-tln) :call EasyMotion#TL(-1,0,0)
xnoremap <silent> <Plug>(easymotion-fl2) :call EasyMotion#SL(2,1,0)
nnoremap <silent> <Plug>(easymotion-fl2) :call EasyMotion#SL(2,0,0)
snoremap <silent> <Plug>(easymotion-fl2) :call EasyMotion#SL(2,0,0)
onoremap <silent> <Plug>(easymotion-fl2) :call EasyMotion#SL(2,0,0)
xnoremap <silent> <Plug>(easymotion-bd-fl2) :call EasyMotion#SL(2,1,2)
nnoremap <silent> <Plug>(easymotion-bd-fl2) :call EasyMotion#SL(2,0,2)
snoremap <silent> <Plug>(easymotion-bd-fl2) :call EasyMotion#SL(2,0,2)
onoremap <silent> <Plug>(easymotion-bd-fl2) :call EasyMotion#SL(2,0,2)
xnoremap <silent> <Plug>(easymotion-T2) :call EasyMotion#T(2,1,1)
nnoremap <silent> <Plug>(easymotion-T2) :call EasyMotion#T(2,0,1)
snoremap <silent> <Plug>(easymotion-T2) :call EasyMotion#T(2,0,1)
onoremap <silent> <Plug>(easymotion-T2) :call EasyMotion#T(2,0,1)
xnoremap <silent> <Plug>(easymotion-bd-tln) :call EasyMotion#TL(-1,1,2)
nnoremap <silent> <Plug>(easymotion-bd-tln) :call EasyMotion#TL(-1,0,2)
snoremap <silent> <Plug>(easymotion-bd-tln) :call EasyMotion#TL(-1,0,2)
onoremap <silent> <Plug>(easymotion-bd-tln) :call EasyMotion#TL(-1,0,2)
xnoremap <silent> <Plug>(easymotion-T) :call EasyMotion#T(1,1,1)
nnoremap <silent> <Plug>(easymotion-T) :call EasyMotion#T(1,0,1)
snoremap <silent> <Plug>(easymotion-T) :call EasyMotion#T(1,0,1)
onoremap <silent> <Plug>(easymotion-T) :call EasyMotion#T(1,0,1)
xnoremap <silent> <Plug>(easymotion-bd-t) :call EasyMotion#T(1,1,2)
nnoremap <silent> <Plug>(easymotion-bd-t) :call EasyMotion#T(1,0,2)
snoremap <silent> <Plug>(easymotion-bd-t) :call EasyMotion#T(1,0,2)
onoremap <silent> <Plug>(easymotion-bd-t) :call EasyMotion#T(1,0,2)
xnoremap <silent> <Plug>(easymotion-Tn) :call EasyMotion#T(-1,1,1)
nnoremap <silent> <Plug>(easymotion-Tn) :call EasyMotion#T(-1,0,1)
snoremap <silent> <Plug>(easymotion-Tn) :call EasyMotion#T(-1,0,1)
onoremap <silent> <Plug>(easymotion-Tn) :call EasyMotion#T(-1,0,1)
xnoremap <silent> <Plug>(easymotion-s2) :call EasyMotion#S(2,1,2)
nnoremap <silent> <Plug>(easymotion-s2) :call EasyMotion#S(2,0,2)
snoremap <silent> <Plug>(easymotion-s2) :call EasyMotion#S(2,0,2)
onoremap <silent> <Plug>(easymotion-s2) :call EasyMotion#S(2,0,2)
xnoremap <silent> <Plug>(easymotion-Tl) :call EasyMotion#TL(1,1,1)
nnoremap <silent> <Plug>(easymotion-Tl) :call EasyMotion#TL(1,0,1)
snoremap <silent> <Plug>(easymotion-Tl) :call EasyMotion#TL(1,0,1)
onoremap <silent> <Plug>(easymotion-Tl) :call EasyMotion#TL(1,0,1)
xnoremap <silent> <Plug>(easymotion-sn) :call EasyMotion#S(-1,1,2)
nnoremap <silent> <Plug>(easymotion-sn) :call EasyMotion#S(-1,0,2)
snoremap <silent> <Plug>(easymotion-sn) :call EasyMotion#S(-1,0,2)
onoremap <silent> <Plug>(easymotion-sn) :call EasyMotion#S(-1,0,2)
xnoremap <silent> <Plug>(easymotion-Fn) :call EasyMotion#S(-1,1,1)
nnoremap <silent> <Plug>(easymotion-Fn) :call EasyMotion#S(-1,0,1)
snoremap <silent> <Plug>(easymotion-Fn) :call EasyMotion#S(-1,0,1)
onoremap <silent> <Plug>(easymotion-Fn) :call EasyMotion#S(-1,0,1)
xnoremap <silent> <Plug>(easymotion-sl) :call EasyMotion#SL(1,1,2)
nnoremap <silent> <Plug>(easymotion-sl) :call EasyMotion#SL(1,0,2)
snoremap <silent> <Plug>(easymotion-sl) :call EasyMotion#SL(1,0,2)
onoremap <silent> <Plug>(easymotion-sl) :call EasyMotion#SL(1,0,2)
xnoremap <silent> <Plug>(easymotion-Fl) :call EasyMotion#SL(1,1,1)
nnoremap <silent> <Plug>(easymotion-Fl) :call EasyMotion#SL(1,0,1)
snoremap <silent> <Plug>(easymotion-Fl) :call EasyMotion#SL(1,0,1)
onoremap <silent> <Plug>(easymotion-Fl) :call EasyMotion#SL(1,0,1)
xnoremap <silent> <Plug>(easymotion-sl2) :call EasyMotion#SL(2,1,2)
nnoremap <silent> <Plug>(easymotion-sl2) :call EasyMotion#SL(2,0,2)
snoremap <silent> <Plug>(easymotion-sl2) :call EasyMotion#SL(2,0,2)
onoremap <silent> <Plug>(easymotion-sl2) :call EasyMotion#SL(2,0,2)
xnoremap <silent> <Plug>(easymotion-bd-fln) :call EasyMotion#SL(-1,1,2)
nnoremap <silent> <Plug>(easymotion-bd-fln) :call EasyMotion#SL(-1,0,2)
snoremap <silent> <Plug>(easymotion-bd-fln) :call EasyMotion#SL(-1,0,2)
onoremap <silent> <Plug>(easymotion-bd-fln) :call EasyMotion#SL(-1,0,2)
xnoremap <silent> <Plug>(easymotion-F) :call EasyMotion#S(1,1,1)
nnoremap <silent> <Plug>(easymotion-F) :call EasyMotion#S(1,0,1)
snoremap <silent> <Plug>(easymotion-F) :call EasyMotion#S(1,0,1)
onoremap <silent> <Plug>(easymotion-F) :call EasyMotion#S(1,0,1)
xnoremap <silent> <Plug>(easymotion-bd-f) :call EasyMotion#S(1,1,2)
nnoremap <silent> <Plug>(easymotion-bd-f) :call EasyMotion#S(1,0,2)
snoremap <silent> <Plug>(easymotion-bd-f) :call EasyMotion#S(1,0,2)
onoremap <silent> <Plug>(easymotion-bd-f) :call EasyMotion#S(1,0,2)
xnoremap <silent> <Plug>(easymotion-F2) :call EasyMotion#S(2,1,1)
nnoremap <silent> <Plug>(easymotion-F2) :call EasyMotion#S(2,0,1)
snoremap <silent> <Plug>(easymotion-F2) :call EasyMotion#S(2,0,1)
onoremap <silent> <Plug>(easymotion-F2) :call EasyMotion#S(2,0,1)
xnoremap <silent> <Plug>(easymotion-bd-f2) :call EasyMotion#S(2,1,2)
nnoremap <silent> <Plug>(easymotion-bd-f2) :call EasyMotion#S(2,0,2)
snoremap <silent> <Plug>(easymotion-bd-f2) :call EasyMotion#S(2,0,2)
onoremap <silent> <Plug>(easymotion-bd-f2) :call EasyMotion#S(2,0,2)
xnoremap <silent> <Plug>(easymotion-Tl2) :call EasyMotion#TL(2,1,1)
nnoremap <silent> <Plug>(easymotion-Tl2) :call EasyMotion#TL(2,0,1)
snoremap <silent> <Plug>(easymotion-Tl2) :call EasyMotion#TL(2,0,1)
onoremap <silent> <Plug>(easymotion-Tl2) :call EasyMotion#TL(2,0,1)
xnoremap <silent> <Plug>(easymotion-fln) :call EasyMotion#SL(-1,1,0)
nnoremap <silent> <Plug>(easymotion-fln) :call EasyMotion#SL(-1,0,0)
snoremap <silent> <Plug>(easymotion-fln) :call EasyMotion#SL(-1,0,0)
onoremap <silent> <Plug>(easymotion-fln) :call EasyMotion#SL(-1,0,0)
xnoremap <silent> <Plug>(openbrowser-smart-search) :call openbrowser#_keymap_smart_search('v')
nnoremap <silent> <Plug>(openbrowser-smart-search) :call openbrowser#_keymap_smart_search('n')
xnoremap <silent> <Plug>(openbrowser-search) :call openbrowser#_keymap_search('v')
nnoremap <silent> <Plug>(openbrowser-search) :call openbrowser#_keymap_search('n')
xnoremap <silent> <Plug>(openbrowser-open-incognito) :call openbrowser#_keymap_open('v', 0, ['--incognito'])
nnoremap <silent> <Plug>(openbrowser-open-incognito) :call openbrowser#_keymap_open('n', 0, ['--incognito'])
xnoremap <silent> <Plug>(openbrowser-open) :call openbrowser#_keymap_open('v')
nnoremap <silent> <Plug>(openbrowser-open) :call openbrowser#_keymap_open('n')
xnoremap <silent> <Plug>(expand_region_shrink) :call expand_region#next('v', '-')
xnoremap <silent> <Plug>(expand_region_expand) :call expand_region#next('v', '+')
nnoremap <silent> <Plug>(expand_region_expand) :call expand_region#next('n', '+')
snoremap <silent> <Plug>(complete_parameter#overload_up) :call cmp#overload_next(0)
nnoremap <silent> <Plug>(complete_parameter#overload_up) :call cmp#overload_next(0)
snoremap <silent> <Plug>(complete_parameter#overload_down) :call cmp#overload_next(1)
nnoremap <silent> <Plug>(complete_parameter#overload_down) :call cmp#overload_next(1)
snoremap <silent> <Plug>(complete_parameter#goto_previous_parameter) :call cmp#goto_next_param(0)
nnoremap <silent> <Plug>(complete_parameter#goto_previous_parameter) :call cmp#goto_next_param(0)
snoremap <silent> <Plug>(complete_parameter#goto_next_parameter) :call cmp#goto_next_param(1)
nnoremap <silent> <Plug>(complete_parameter#goto_next_parameter) :call cmp#goto_next_param(1)
nmap <silent> <2-LeftMouse> <Plug>(matchup-double-click)
nnoremap <Plug>(matchup-reload) :MatchupReload
nnoremap <silent> <Plug>(matchup-double-click) :call matchup#text_obj#double_click()
onoremap <silent> <Plug>(matchup-a%) :call matchup#text_obj#delimited(0, 0, 'delim_all')
onoremap <silent> <Plug>(matchup-i%) :call matchup#text_obj#delimited(1, 0, 'delim_all')
xnoremap <silent> <Plug>(matchup-a%) :call matchup#text_obj#delimited(0, 1, 'delim_all')
xnoremap <silent> <Plug>(matchup-i%) :call matchup#text_obj#delimited(1, 1, 'delim_all')
onoremap <silent> <Plug>(matchup-z%) :call matchup#motion#op('z%')
xnoremap <silent> <SNR>114_(matchup-z%) :call matchup#motion#jump_inside(1)
nnoremap <silent> <Plug>(matchup-z%) :call matchup#motion#jump_inside(0)
onoremap <silent> <Plug>(matchup-[%) :call matchup#motion#op('[%')
onoremap <silent> <Plug>(matchup-]%) :call matchup#motion#op(']%')
xnoremap <silent> <SNR>114_(matchup-[%) :call matchup#motion#find_unmatched(1, 0)
xnoremap <silent> <SNR>114_(matchup-]%) :call matchup#motion#find_unmatched(1, 1)
nnoremap <silent> <Plug>(matchup-[%) :call matchup#motion#find_unmatched(0, 0)
nnoremap <silent> <Plug>(matchup-]%) :call matchup#motion#find_unmatched(0, 1)
onoremap <silent> <Plug>(matchup-g%) :call matchup#motion#op('g%')
xnoremap <silent> <SNR>114_(matchup-g%) :call matchup#motion#find_matching_pair(1, 0)
onoremap <silent> <Plug>(matchup-%) :call matchup#motion#op('%')
xnoremap <silent> <SNR>114_(matchup-%) :call matchup#motion#find_matching_pair(1, 1)
nnoremap <silent> <Plug>(matchup-g%) :call matchup#motion#find_matching_pair(0, 0)
nnoremap <silent> <Plug>(matchup-%) :call matchup#motion#find_matching_pair(0, 1)
nnoremap <silent> <expr> <SNR>114_(wise) empty(g:v_motion_force) ? 'v' : g:v_motion_force
nnoremap <silent> <Plug>(matchup-hi-surround) :call matchup#matchparen#highlight_surrounding()
map <PageUp> <Plug>(SmoothieBackwards)
map <S-Up> <Plug>(SmoothieBackwards)
map <C-B> <Plug>(SmoothieBackwards)
map <PageDown> <Plug>(SmoothieForwards)
map <S-Down> <Plug>(SmoothieForwards)
map <C-F> <Plug>(SmoothieForwards)
map <C-U> <Plug>(SmoothieUpwards)
map <C-D> <Plug>(SmoothieDownwards)
noremap <silent> <Plug>(Smoothie_G) <Cmd>call smoothie#cursor_movement('G')  
noremap <silent> <Plug>(Smoothie_gg) <Cmd>call smoothie#cursor_movement('gg') 
noremap <silent> <Plug>(SmoothieBackwards) <Cmd>call smoothie#backwards()           
noremap <silent> <Plug>(SmoothieForwards) <Cmd>call smoothie#forwards()            
noremap <silent> <Plug>(SmoothieUpwards) <Cmd>call smoothie#upwards()             
noremap <silent> <Plug>(SmoothieDownwards) <Cmd>call smoothie#downwards()           
xmap <silent> <Plug>(easymotion-prefix)l <Plug>(easyoperator-line-select)
omap <silent> <Plug>(easymotion-prefix)l <Plug>(easyoperator-line-select)
nnoremap <Plug>(easyoperator-line-yank) :call easyoperator#line#selectlinesyank()
nnoremap <Plug>(easyoperator-line-delete) :call easyoperator#line#selectlinesdelete()
xnoremap <Plug>(easyoperator-line-select) :call easyoperator#line#selectlines()
onoremap <Plug>(easyoperator-line-select) :call easyoperator#line#selectlines()
nnoremap <Plug>(easyoperator-line-select) :call easyoperator#line#selectlines()
nnoremap <silent> <Plug>(grammarous-move-to-previous-error) :call grammarous#move_to_previous_error(getpos('.')[1 : 2], b:grammarous_result)
nnoremap <silent> <Plug>(grammarous-move-to-next-error) :call grammarous#move_to_next_error(getpos('.')[1 : 2], b:grammarous_result)
nnoremap <silent> <Plug>(grammarous-disable-category) :call grammarous#disable_category_at(getpos('.')[1 : 2], b:grammarous_result)
nnoremap <silent> <Plug>(grammarous-disable-rule) :call grammarous#disable_rule_at(getpos('.')[1 : 2], b:grammarous_result)
nnoremap <silent> <Plug>(grammarous-remove-error) :call grammarous#remove_error_at(getpos('.')[1 : 2], b:grammarous_result)
nnoremap <silent> <Plug>(grammarous-close-info-window) :call grammarous#info_win#close()
nnoremap <silent> <Plug>(grammarous-fixall) :call grammarous#fixall(b:grammarous_result)
nnoremap <silent> <Plug>(grammarous-fixit) :call grammarous#fixit(grammarous#get_error_at(getpos('.')[1 : 2], b:grammarous_result))
nnoremap <silent> <Plug>(grammarous-reset) :call grammarous#reset()
nnoremap <silent> <Plug>(grammarous-open-info-window) :call grammarous#create_update_info_window_of(b:grammarous_result)
nnoremap <silent> <Plug>(grammarous-move-to-info-window) :call grammarous#create_and_jump_to_info_window_of(b:grammarous_result)
tnoremap <silent> <C-`> :q
tnoremap <silent> <M-Right> :bnext
tnoremap <silent> <M-Left> :bprev
tnoremap <silent> <C-Down> :wincmd j
tnoremap <silent> <C-Up> :wincmd k
tnoremap <silent> <C-Left> :wincmd h
tnoremap <silent> <C-Right> :wincmd l
vnoremap <silent> <Plug>YankGitRemoteURL :call SpaceVim#util#CopyToClipboard(3)
nnoremap <silent> <Plug>YankGitRemoteURL :call SpaceVim#util#CopyToClipboard(2)
nnoremap <silent> <S-Tab> :wincmd p
noremap <silent> <F3> :NERDTreeToggle
noremap <silent> <F2> :TagbarToggle
vnoremap <silent> <Plug>ReverseLines :ReverseLines
nnoremap <silent> <Plug>ReverseLines :ReverseLines
nnoremap <silent> <F7> :MundoToggle
vnoremap <C-Space> <Plug>(wildfire-water)
xnoremap <silent> <Plug>(jplus :call dein#autoload#_on_map('<Plug>(jplus', 'vim-jplus','x')
nnoremap <silent> <Plug>(jplus :call dein#autoload#_on_map('<Plug>(jplus', 'vim-jplus','n')
xnoremap <silent> <Plug>(wildfire- :call dein#autoload#_on_map('<Plug>(wildfire-', 'wildfire.vim','x')
nnoremap <silent> <Plug>(wildfire- :call dein#autoload#_on_map('<Plug>(wildfire-', 'wildfire.vim','n')
xnoremap <silent> <Plug>SpaceVim-plugin-iedit :call SpaceVim#plugins#iedit#start(1)
nnoremap <silent> <Plug>SpaceVim-plugin-iedit :call SpaceVim#plugins#iedit#start()
vnoremap <C-S> :w
nnoremap <C-S> :w
nnoremap <silent> <Up> gk
nnoremap <silent> <Down> gj
xnoremap <S-Tab> <gv
nnoremap <silent> <C-`> :call SpaceVim#plugins#runner#close()
nnoremap <silent> <C-G> :call SpaceVim#plugins#ctrlg#display()
vnoremap <silent> <C-S-Up> :m '<-2gv=gv
vnoremap <silent> <C-S-Down> :m '>+1gv=gv
nnoremap <silent> <C-S-Up> :m .-2==
nnoremap <silent> <C-S-Down> :m .+1==
nnoremap <silent> <C-Down> :wincmd j
nnoremap <silent> <C-Up> :wincmd k
nnoremap <silent> <C-Left> :wincmd h
nnoremap <silent> <C-Right> :wincmd l
imap S <Plug>ISurround
imap s <Plug>Isurround
imap <silent> % <Plug>(matchup-c_g%)
inoremap <expr> 	 pumvisible() ? "\<Down>" : "\	"
imap <silent> <expr>  pumvisible() ? "\" : "\"
imap  <Plug>Isurround
cnoremap  w
imap <expr> ( pumvisible() ? has('patch-7.4.744') ? complete_parameter#pre_complete("()") : '(' : (len(maparg('<Plug>delimitMate(', 'i')) == 0) ? "\<Plug>delimitMate(" : '('
let &cpo=s:cpo_save
unlet s:cpo_save
set autoindent
set autoread
set background=dark
set backspace=indent,eol,start
set backup
set backupdir=~/.cache/SpaceVim/old/backup//
set cindent
set complete=.,w,b,u,t
set completeopt=menu,menuone,longest
set cpoptions=aABceFsd
set directory=~/.cache/SpaceVim/old/swap//
set display=lastline
set expandtab
set fileencodings=utf-8,ucs-bom,gb18030,gbk,gb2312,cp936
set fillchars=vert:│,fold:·
set helplang=en
set hidden
set hlsearch
set incsearch
set laststatus=2
set listchars=tab:→\ ,eol:↵,trail:·,extends:↷,precedes:↶
set matchtime=0
set nomodeline
set mouse=nv
set nrformats=bin,hex
set printoptions=paper:letter
set pumheight=15
set runtimepath=~/.SpaceVim.d/,~/.SpaceVim/,~/.SpaceVim/bundle/indentLine,~/.SpaceVim/bundle/vim-repeat,~/.SpaceVim/bundle/vim-grammarous,~/.SpaceVim/bundle/vim-textobj-line,~/.SpaceVim/bundle/vim-hug-neovim-rpc,~/.SpaceVim/bundle/vim-emoji,~/.SpaceVim/bundle/vim-easyoperator-line,~/.SpaceVim/bundle/vim-cursorword,~/.SpaceVim/bundle/neomake,~/.SpaceVim/bundle/neosnippet-snippets,~/.SpaceVim/bundle/tagbar,~/.SpaceVim/bundle/nvim-if-lua-compat,~/.SpaceVim/bundle/vim-smoothie,~/.SpaceVim/bundle/vim-textobj-indent,~/.SpaceVim/bundle/vim-matchup,~/.SpaceVim/bundle/gruvbox,~/.SpaceVim/bundle/CompleteParameter.vim,~/.SpaceVim/bundle/editorconfig-vim,~/.SpaceVim/bundle/delimitMate,~/.SpaceVim/bundle/vim-expand-region,~/.SpaceVim/bundle/vim-textobj-user,~/.SpaceVim/bundle/nerdtree,~/.SpaceVim/bundle/open-browser.vim,~/.SpaceVim/bundle/tagbar-proto.vim,~/.SpaceVim/bundle/tabular,~/.SpaceVim/bundle/vim-easymotion,~/.SpaceVim/bundle/nvim-yarp,~/.SpaceVim/bundle/vim-startify,~/.SpaceVim/bundle/tagbar-makefile.vim,~/.SpaceVim/bundle/dein.vim,~/.SpaceVim/bundle/vim-table-mode,~/.SpaceVim/bundle/neoformat,~/.SpaceVim/bundle/vim-textobj-entire,~/.SpaceVim/bundle/vim-surround,~/.SpaceVim/bundle/deoplete-dictionary,~/.SpaceVim/bundle/vim-clipboard,~/.SpaceVim/bundle/nerdcommenter,~/.cache/vimfiles/.cache/vimrc/.dein,/usr/share/vim/vim91,~/.SpaceVim/bundle/indentLine/after,~/.SpaceVim/bundle/vim-matchup/after,~/.SpaceVim/bundle/CompleteParameter.vim/after,~/.SpaceVim/bundle/tabular/after,~/.cache/vimfiles/.cache/vimrc/.dein/after,~/.SpaceVim.d/after,~/.SpaceVim/bundle/dein.vim/,~/.SpaceVim/after
set scrolloff=1
set noshelltemp
set shiftwidth=4
set shortmess=filnxtToOScsF
set showmatch
set noshowmode
set showtabline=2
set sidescrolloff=5
set smartindent
set smarttab
set softtabstop=4
set suffixes=.bak,~,.swp,.o,.info,.aux,.log,.dvi,.bbl,.blg,.brf,.cb,.ind,.idx,.ilg,.inx,.out,.toc
set tabline=%!SpaceVim#layers#core#tabline#get()
set tabstop=4
set termencoding=utf-8
set ttimeout
set ttimeoutlen=50
set undodir=~/.cache/SpaceVim/old/undofile//
set undofile
set wildignore=*/tmp/*,*.so,*.swp,*.zip,*.class,tags,*.jpg,*.ttf,*.TTF,*.png,*/target/*,.git,.svn,.hg,.DS_Store,*.svg
set wildignorecase
set wildmenu
set nowritebackup
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/RaylibEngine/src
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +0 Editor.cpp
badd +0 Rendering.h
badd +0 Rendering.cpp
badd +0 GameManager.h
badd +0 GameManager.cpp
badd +0 GameObject.h
badd +0 GameObject.cpp
badd +0 Entity.h
badd +0 Entity.cpp
argglobal
%argdel
$argadd Editor.cpp
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabrewind
edit Editor.cpp
argglobal
let s:cpo_save=&cpo
set cpo&vim
imap <buffer> <silent> <C-G>g <Plug>delimitMateJumpMany
imap <buffer> <S-BS> <Plug>delimitMateS-BS
imap <buffer> <C-H> <Plug>delimitMateBS
imap <buffer> <BS> <Plug>delimitMateBS
imap <buffer> <silent> g <Plug>delimitMateJumpMany
imap <buffer>  <Plug>delimitMateBS
imap <buffer> " <Plug>delimitMate"
imap <buffer> ' <Plug>delimitMate'
imap <buffer> ( <Plug>delimitMate(
imap <buffer> ) <Plug>delimitMate)
imap <buffer> [ <Plug>delimitMate[
imap <buffer> ] <Plug>delimitMate]
imap <buffer> ` <Plug>delimitMate`
imap <buffer> { <Plug>delimitMate{
imap <buffer> } <Plug>delimitMate}
let &cpo=s:cpo_save
unlet s:cpo_save
setlocal keymap=
setlocal noarabic
setlocal autoindent
setlocal backupcopy=
setlocal balloonexpr=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinscopedecls=public,protected,private
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,:///,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t
setlocal completefunc=
setlocal concealcursor=
setlocal conceallevel=0
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal cursorlineopt=both
setlocal define=^\\s*#\\s*define
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal expandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fillchars=
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
set foldtext=SpaceVim#default#Customfoldtext()
setlocal foldtext=SpaceVim#default#Customfoldtext()
setlocal formatexpr=
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal formatoptions=croql
setlocal formatprg=
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=-1
setlocal include=^\\s*#\\s*include
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
set linebreak
setlocal linebreak
setlocal nolisp
setlocal lispoptions=
setlocal lispwords=
setlocal nolist
setlocal listchars=
setlocal makeencoding=
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal nomodeline
setlocal modifiable
setlocal nrformats=bin,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=ccomplete#Complete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal scrolloff=-1
setlocal shiftwidth=4
setlocal noshortname
setlocal showbreak=
setlocal sidescrolloff=-1
setlocal signcolumn=auto
setlocal smartindent
setlocal nosmoothscroll
setlocal softtabstop=4
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal spelloptions=
setlocal statusline=%#SpaceVim_statusline_ia#\ %{\ get(w:,\ \"winid\",\ winnr())\ }\ %#SpaceVim_statusline_ia_SpaceVim_statusline_b#%#SpaceVim_statusline_b#%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 38\ ?\ \"\"\ :\ (\"\ 448\ bytes\ Editor.cpp\ \ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 46\ ?\ \"\"\ :\ (\"\ \ cpp\ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 56\ ?\ \"\"\ :\ (\"\ \ #\ s\ \ \ |\")}%=|%{\"\ \"\ .\ &ff\ .\ \"|\"\ .\ (&fenc!=\"\"?&fenc:&enc)\ .\ \"\ \"}|\ %P\ 
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=4
setlocal tagcase=
setlocal tagfunc=
setlocal tags=
setlocal termwinkey=
setlocal termwinscroll=10000
setlocal termwinsize=
setlocal textwidth=0
setlocal thesaurus=
setlocal thesaurusfunc=
setlocal undofile
setlocal undolevels=-123456
setlocal varsofttabstop=
setlocal vartabstop=
setlocal virtualedit=
setlocal wincolor=
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let &fdl = &fdl
let s:l = 1 - ((0 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
tabnext
edit Rendering.cpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 95 + 95) / 190)
exe 'vert 2resize ' . ((&columns * 94 + 95) / 190)
argglobal
balt Rendering.h
let s:cpo_save=&cpo
set cpo&vim
imap <buffer> <silent> <C-G>g <Plug>delimitMateJumpMany
imap <buffer> <S-BS> <Plug>delimitMateS-BS
imap <buffer> <C-H> <Plug>delimitMateBS
imap <buffer> <BS> <Plug>delimitMateBS
imap <buffer> <silent> g <Plug>delimitMateJumpMany
imap <buffer>  <Plug>delimitMateBS
imap <buffer> " <Plug>delimitMate"
imap <buffer> ' <Plug>delimitMate'
imap <buffer> ( <Plug>delimitMate(
imap <buffer> ) <Plug>delimitMate)
imap <buffer> [ <Plug>delimitMate[
imap <buffer> ] <Plug>delimitMate]
imap <buffer> ` <Plug>delimitMate`
imap <buffer> { <Plug>delimitMate{
imap <buffer> } <Plug>delimitMate}
let &cpo=s:cpo_save
unlet s:cpo_save
setlocal keymap=
setlocal noarabic
setlocal autoindent
setlocal backupcopy=
setlocal balloonexpr=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinscopedecls=public,protected,private
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,:///,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t
setlocal completefunc=
setlocal concealcursor=
setlocal conceallevel=0
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal cursorlineopt=both
setlocal define=^\\s*#\\s*define
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal expandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fillchars=
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
set foldtext=SpaceVim#default#Customfoldtext()
setlocal foldtext=SpaceVim#default#Customfoldtext()
setlocal formatexpr=
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal formatoptions=croql
setlocal formatprg=
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=-1
setlocal include=^\\s*#\\s*include
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
set linebreak
setlocal linebreak
setlocal nolisp
setlocal lispoptions=
setlocal lispwords=
setlocal nolist
setlocal listchars=
setlocal makeencoding=
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal nomodeline
setlocal modifiable
setlocal nrformats=bin,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=ccomplete#Complete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal scrolloff=-1
setlocal shiftwidth=4
setlocal noshortname
setlocal showbreak=
setlocal sidescrolloff=-1
setlocal signcolumn=auto
setlocal smartindent
setlocal nosmoothscroll
setlocal softtabstop=4
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal spelloptions=
setlocal statusline=%#SpaceVim_statusline_ia#\ %{\ get(w:,\ \"winid\",\ winnr())\ }\ %#SpaceVim_statusline_ia_SpaceVim_statusline_b#%#SpaceVim_statusline_b#%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 41\ ?\ \"\"\ :\ (\"\ 584\ bytes\ Rendering.cpp\ \ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 49\ ?\ \"\"\ :\ (\"\ \ cpp\ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 59\ ?\ \"\"\ :\ (\"\ \ #\ s\ \ \ |\")}%=|%{\"\ \"\ .\ &ff\ .\ \"|\"\ .\ (&fenc!=\"\"?&fenc:&enc)\ .\ \"\ \"}|\ %P\ 
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=4
setlocal tagcase=
setlocal tagfunc=
setlocal tags=
setlocal termwinkey=
setlocal termwinscroll=10000
setlocal termwinsize=
setlocal textwidth=0
setlocal thesaurus=
setlocal thesaurusfunc=
setlocal undofile
setlocal undolevels=-123456
setlocal varsofttabstop=
setlocal vartabstop=
setlocal virtualedit=
setlocal wincolor=
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let &fdl = &fdl
let s:l = 22 - ((21 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 22
normal! 08|
wincmd w
argglobal
if bufexists(fnamemodify("Rendering.h", ":p")) | buffer Rendering.h | else | edit Rendering.h | endif
balt Rendering.cpp
let s:cpo_save=&cpo
set cpo&vim
imap <buffer> <silent> <C-G>g <Plug>delimitMateJumpMany
imap <buffer> <S-BS> <Plug>delimitMateS-BS
imap <buffer> <C-H> <Plug>delimitMateBS
imap <buffer> <BS> <Plug>delimitMateBS
imap <buffer> <silent> g <Plug>delimitMateJumpMany
imap <buffer>  <Plug>delimitMateBS
imap <buffer> " <Plug>delimitMate"
imap <buffer> ' <Plug>delimitMate'
imap <buffer> ( <Plug>delimitMate(
imap <buffer> ) <Plug>delimitMate)
imap <buffer> [ <Plug>delimitMate[
imap <buffer> ] <Plug>delimitMate]
imap <buffer> ` <Plug>delimitMate`
imap <buffer> { <Plug>delimitMate{
imap <buffer> } <Plug>delimitMate}
let &cpo=s:cpo_save
unlet s:cpo_save
setlocal keymap=
setlocal noarabic
setlocal autoindent
setlocal backupcopy=
setlocal balloonexpr=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinscopedecls=public,protected,private
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,:///,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t
setlocal completefunc=
setlocal concealcursor=
setlocal conceallevel=0
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal cursorlineopt=both
setlocal define=^\\s*#\\s*define
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal expandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fillchars=
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
set foldtext=SpaceVim#default#Customfoldtext()
setlocal foldtext=SpaceVim#default#Customfoldtext()
setlocal formatexpr=
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal formatoptions=croql
setlocal formatprg=
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=-1
setlocal include=^\\s*#\\s*include
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
set linebreak
setlocal linebreak
setlocal nolisp
setlocal lispoptions=
setlocal lispwords=
setlocal nolist
setlocal listchars=
setlocal makeencoding=
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal nomodeline
setlocal modifiable
setlocal nrformats=bin,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=ccomplete#Complete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal scrolloff=-1
setlocal shiftwidth=4
setlocal noshortname
setlocal showbreak=
setlocal sidescrolloff=-1
setlocal signcolumn=auto
setlocal smartindent
setlocal nosmoothscroll
setlocal softtabstop=4
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal spelloptions=
setlocal statusline=%#SpaceVim_statusline_ia#\ %{\ get(w:,\ \"winid\",\ winnr())\ }\ %#SpaceVim_statusline_ia_SpaceVim_statusline_b#%#SpaceVim_statusline_b#%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 39\ ?\ \"\"\ :\ (\"\ 455\ bytes\ Rendering.h\ \ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 47\ ?\ \"\"\ :\ (\"\ \ cpp\ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 57\ ?\ \"\"\ :\ (\"\ \ #\ s\ \ \ |\")}%=|%{\"\ \"\ .\ &ff\ .\ \"|\"\ .\ (&fenc!=\"\"?&fenc:&enc)\ .\ \"\ \"}|\ %P\ 
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=4
setlocal tagcase=
setlocal tagfunc=
setlocal tags=
setlocal termwinkey=
setlocal termwinscroll=10000
setlocal termwinsize=
setlocal textwidth=0
setlocal thesaurus=
setlocal thesaurusfunc=
setlocal undofile
setlocal undolevels=-123456
setlocal varsofttabstop=
setlocal vartabstop=
setlocal virtualedit=
setlocal wincolor=
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let &fdl = &fdl
let s:l = 19 - ((18 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 19
normal! 026|
wincmd w
exe 'vert 1resize ' . ((&columns * 95 + 95) / 190)
exe 'vert 2resize ' . ((&columns * 94 + 95) / 190)
tabnext
edit GameManager.cpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 95 + 95) / 190)
exe 'vert 2resize ' . ((&columns * 94 + 95) / 190)
argglobal
balt GameManager.h
let s:cpo_save=&cpo
set cpo&vim
imap <buffer> <silent> <C-G>g <Plug>delimitMateJumpMany
imap <buffer> <S-BS> <Plug>delimitMateS-BS
imap <buffer> <C-H> <Plug>delimitMateBS
imap <buffer> <BS> <Plug>delimitMateBS
imap <buffer> <silent> g <Plug>delimitMateJumpMany
imap <buffer>  <Plug>delimitMateBS
imap <buffer> " <Plug>delimitMate"
imap <buffer> ' <Plug>delimitMate'
imap <buffer> ( <Plug>delimitMate(
imap <buffer> ) <Plug>delimitMate)
imap <buffer> [ <Plug>delimitMate[
imap <buffer> ] <Plug>delimitMate]
imap <buffer> ` <Plug>delimitMate`
imap <buffer> { <Plug>delimitMate{
imap <buffer> } <Plug>delimitMate}
let &cpo=s:cpo_save
unlet s:cpo_save
setlocal keymap=
setlocal noarabic
setlocal autoindent
setlocal backupcopy=
setlocal balloonexpr=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinscopedecls=public,protected,private
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,:///,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t
setlocal completefunc=
setlocal concealcursor=
setlocal conceallevel=0
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal cursorlineopt=both
setlocal define=^\\s*#\\s*define
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal expandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fillchars=
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
set foldtext=SpaceVim#default#Customfoldtext()
setlocal foldtext=SpaceVim#default#Customfoldtext()
setlocal formatexpr=
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal formatoptions=croql
setlocal formatprg=
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=-1
setlocal include=^\\s*#\\s*include
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
set linebreak
setlocal linebreak
setlocal nolisp
setlocal lispoptions=
setlocal lispwords=
setlocal nolist
setlocal listchars=
setlocal makeencoding=
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal nomodeline
setlocal modifiable
setlocal nrformats=bin,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=ccomplete#Complete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal scrolloff=-1
setlocal shiftwidth=4
setlocal noshortname
setlocal showbreak=
setlocal sidescrolloff=-1
setlocal signcolumn=auto
setlocal smartindent
setlocal nosmoothscroll
setlocal softtabstop=4
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal spelloptions=
setlocal statusline=%#SpaceVim_statusline_ia#\ %{\ get(w:,\ \"winid\",\ winnr())\ }\ %#SpaceVim_statusline_ia_SpaceVim_statusline_b#%#SpaceVim_statusline_b#%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 43\ ?\ \"\"\ :\ (\"\ 790\ bytes\ GameManager.cpp\ \ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 51\ ?\ \"\"\ :\ (\"\ \ cpp\ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 61\ ?\ \"\"\ :\ (\"\ \ #\ s\ \ \ |\")}%=|%{\"\ \"\ .\ &ff\ .\ \"|\"\ .\ (&fenc!=\"\"?&fenc:&enc)\ .\ \"\ \"}|\ %P\ 
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=4
setlocal tagcase=
setlocal tagfunc=
setlocal tags=
setlocal termwinkey=
setlocal termwinscroll=10000
setlocal termwinsize=
setlocal textwidth=0
setlocal thesaurus=
setlocal thesaurusfunc=
setlocal undofile
setlocal undolevels=-123456
setlocal varsofttabstop=
setlocal vartabstop=
setlocal virtualedit=
setlocal wincolor=
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let &fdl = &fdl
let s:l = 13 - ((12 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 13
normal! 033|
wincmd w
argglobal
if bufexists(fnamemodify("GameManager.h", ":p")) | buffer GameManager.h | else | edit GameManager.h | endif
balt GameManager.cpp
let s:cpo_save=&cpo
set cpo&vim
imap <buffer> <silent> <C-G>g <Plug>delimitMateJumpMany
imap <buffer> <S-BS> <Plug>delimitMateS-BS
imap <buffer> <C-H> <Plug>delimitMateBS
imap <buffer> <BS> <Plug>delimitMateBS
imap <buffer> <silent> g <Plug>delimitMateJumpMany
imap <buffer>  <Plug>delimitMateBS
imap <buffer> " <Plug>delimitMate"
imap <buffer> ' <Plug>delimitMate'
imap <buffer> ( <Plug>delimitMate(
imap <buffer> ) <Plug>delimitMate)
imap <buffer> [ <Plug>delimitMate[
imap <buffer> ] <Plug>delimitMate]
imap <buffer> ` <Plug>delimitMate`
imap <buffer> { <Plug>delimitMate{
imap <buffer> } <Plug>delimitMate}
let &cpo=s:cpo_save
unlet s:cpo_save
setlocal keymap=
setlocal noarabic
setlocal autoindent
setlocal backupcopy=
setlocal balloonexpr=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinscopedecls=public,protected,private
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,:///,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t
setlocal completefunc=
setlocal concealcursor=
setlocal conceallevel=0
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal cursorlineopt=both
setlocal define=^\\s*#\\s*define
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal expandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fillchars=
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
set foldtext=SpaceVim#default#Customfoldtext()
setlocal foldtext=SpaceVim#default#Customfoldtext()
setlocal formatexpr=
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal formatoptions=croql
setlocal formatprg=
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=-1
setlocal include=^\\s*#\\s*include
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
set linebreak
setlocal linebreak
setlocal nolisp
setlocal lispoptions=
setlocal lispwords=
setlocal nolist
setlocal listchars=
setlocal makeencoding=
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal nomodeline
setlocal modifiable
setlocal nrformats=bin,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=ccomplete#Complete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal scrolloff=-1
setlocal shiftwidth=4
setlocal noshortname
setlocal showbreak=
setlocal sidescrolloff=-1
setlocal signcolumn=auto
setlocal smartindent
setlocal nosmoothscroll
setlocal softtabstop=4
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal spelloptions=
setlocal statusline=%#SpaceVim_statusline_ia#\ %{\ get(w:,\ \"winid\",\ winnr())\ }\ %#SpaceVim_statusline_ia_SpaceVim_statusline_b#%#SpaceVim_statusline_b#%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 41\ ?\ \"\"\ :\ (\"\ 764\ bytes\ GameManager.h\ \ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 49\ ?\ \"\"\ :\ (\"\ \ cpp\ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 59\ ?\ \"\"\ :\ (\"\ \ #\ s\ \ \ |\")}%=|%{\"\ \"\ .\ &ff\ .\ \"|\"\ .\ (&fenc!=\"\"?&fenc:&enc)\ .\ \"\ \"}|\ %P\ 
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=4
setlocal tagcase=
setlocal tagfunc=
setlocal tags=
setlocal termwinkey=
setlocal termwinscroll=10000
setlocal termwinsize=
setlocal textwidth=0
setlocal thesaurus=
setlocal thesaurusfunc=
setlocal undofile
setlocal undolevels=-123456
setlocal varsofttabstop=
setlocal vartabstop=
setlocal virtualedit=
setlocal wincolor=
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let &fdl = &fdl
let s:l = 30 - ((29 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 30
normal! 012|
wincmd w
exe 'vert 1resize ' . ((&columns * 95 + 95) / 190)
exe 'vert 2resize ' . ((&columns * 94 + 95) / 190)
tabnext
edit GameObject.cpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 95 + 95) / 190)
exe 'vert 2resize ' . ((&columns * 94 + 95) / 190)
argglobal
balt GameObject.h
let s:cpo_save=&cpo
set cpo&vim
imap <buffer> <silent> <C-G>g <Plug>delimitMateJumpMany
imap <buffer> <S-BS> <Plug>delimitMateS-BS
imap <buffer> <C-H> <Plug>delimitMateBS
imap <buffer> <BS> <Plug>delimitMateBS
imap <buffer> <silent> g <Plug>delimitMateJumpMany
imap <buffer>  <Plug>delimitMateBS
imap <buffer> " <Plug>delimitMate"
imap <buffer> ' <Plug>delimitMate'
imap <buffer> ( <Plug>delimitMate(
imap <buffer> ) <Plug>delimitMate)
imap <buffer> [ <Plug>delimitMate[
imap <buffer> ] <Plug>delimitMate]
imap <buffer> ` <Plug>delimitMate`
imap <buffer> { <Plug>delimitMate{
imap <buffer> } <Plug>delimitMate}
let &cpo=s:cpo_save
unlet s:cpo_save
setlocal keymap=
setlocal noarabic
setlocal autoindent
setlocal backupcopy=
setlocal balloonexpr=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinscopedecls=public,protected,private
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,:///,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t
setlocal completefunc=
setlocal concealcursor=
setlocal conceallevel=0
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal cursorlineopt=both
setlocal define=^\\s*#\\s*define
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal expandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fillchars=
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
set foldtext=SpaceVim#default#Customfoldtext()
setlocal foldtext=SpaceVim#default#Customfoldtext()
setlocal formatexpr=
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal formatoptions=croql
setlocal formatprg=
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=-1
setlocal include=^\\s*#\\s*include
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
set linebreak
setlocal linebreak
setlocal nolisp
setlocal lispoptions=
setlocal lispwords=
setlocal nolist
setlocal listchars=
setlocal makeencoding=
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal nomodeline
setlocal modifiable
setlocal nrformats=bin,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=ccomplete#Complete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal scrolloff=-1
setlocal shiftwidth=4
setlocal noshortname
setlocal showbreak=
setlocal sidescrolloff=-1
setlocal signcolumn=auto
setlocal smartindent
setlocal nosmoothscroll
setlocal softtabstop=4
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal spelloptions=
setlocal statusline=%#SpaceVim_statusline_ia#\ %{\ get(w:,\ \"winid\",\ winnr())\ }\ %#SpaceVim_statusline_ia_SpaceVim_statusline_b#%#SpaceVim_statusline_b#%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 37\ ?\ \"\"\ :\ (\"\ 1.0k\ GameObject.cpp\ \ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 45\ ?\ \"\"\ :\ (\"\ \ cpp\ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 55\ ?\ \"\"\ :\ (\"\ \ #\ s\ \ \ |\")}%=|%{\"\ \"\ .\ &ff\ .\ \"|\"\ .\ (&fenc!=\"\"?&fenc:&enc)\ .\ \"\ \"}|\ %P\ 
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=4
setlocal tagcase=
setlocal tagfunc=
setlocal tags=
setlocal termwinkey=
setlocal termwinscroll=10000
setlocal termwinsize=
setlocal textwidth=0
setlocal thesaurus=
setlocal thesaurusfunc=
setlocal undofile
setlocal undolevels=-123456
setlocal varsofttabstop=
setlocal vartabstop=
setlocal virtualedit=
setlocal wincolor=
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let &fdl = &fdl
let s:l = 42 - ((34 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 42
normal! 045|
wincmd w
argglobal
if bufexists(fnamemodify("GameObject.h", ":p")) | buffer GameObject.h | else | edit GameObject.h | endif
balt GameObject.cpp
let s:cpo_save=&cpo
set cpo&vim
imap <buffer> <silent> <C-G>g <Plug>delimitMateJumpMany
imap <buffer> <S-BS> <Plug>delimitMateS-BS
imap <buffer> <C-H> <Plug>delimitMateBS
imap <buffer> <BS> <Plug>delimitMateBS
imap <buffer> <silent> g <Plug>delimitMateJumpMany
imap <buffer>  <Plug>delimitMateBS
imap <buffer> " <Plug>delimitMate"
imap <buffer> ' <Plug>delimitMate'
imap <buffer> ( <Plug>delimitMate(
imap <buffer> ) <Plug>delimitMate)
imap <buffer> [ <Plug>delimitMate[
imap <buffer> ] <Plug>delimitMate]
imap <buffer> ` <Plug>delimitMate`
imap <buffer> { <Plug>delimitMate{
imap <buffer> } <Plug>delimitMate}
let &cpo=s:cpo_save
unlet s:cpo_save
setlocal keymap=
setlocal noarabic
setlocal autoindent
setlocal backupcopy=
setlocal balloonexpr=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinscopedecls=public,protected,private
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,:///,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t
setlocal completefunc=
setlocal concealcursor=
setlocal conceallevel=0
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal cursorlineopt=both
setlocal define=^\\s*#\\s*define
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal expandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fillchars=
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
set foldtext=SpaceVim#default#Customfoldtext()
setlocal foldtext=SpaceVim#default#Customfoldtext()
setlocal formatexpr=
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal formatoptions=croql
setlocal formatprg=
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=-1
setlocal include=^\\s*#\\s*include
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
set linebreak
setlocal linebreak
setlocal nolisp
setlocal lispoptions=
setlocal lispwords=
setlocal nolist
setlocal listchars=
setlocal makeencoding=
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal nomodeline
setlocal modifiable
setlocal nrformats=bin,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=ccomplete#Complete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal scrolloff=-1
setlocal shiftwidth=4
setlocal noshortname
setlocal showbreak=
setlocal sidescrolloff=-1
setlocal signcolumn=auto
setlocal smartindent
setlocal nosmoothscroll
setlocal softtabstop=4
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal spelloptions=
setlocal statusline=%#SpaceVim_statusline_ia#\ %{\ get(w:,\ \"winid\",\ winnr())\ }\ %#SpaceVim_statusline_ia_SpaceVim_statusline_b#%#SpaceVim_statusline_b#%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 40\ ?\ \"\"\ :\ (\"\ 758\ bytes\ GameObject.h\ \ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 48\ ?\ \"\"\ :\ (\"\ \ cpp\ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 58\ ?\ \"\"\ :\ (\"\ \ #\ s\ \ \ |\")}%=|%{\"\ \"\ .\ &ff\ .\ \"|\"\ .\ (&fenc!=\"\"?&fenc:&enc)\ .\ \"\ \"}|\ %P\ 
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=4
setlocal tagcase=
setlocal tagfunc=
setlocal tags=
setlocal termwinkey=
setlocal termwinscroll=10000
setlocal termwinsize=
setlocal textwidth=0
setlocal thesaurus=
setlocal thesaurusfunc=
setlocal undofile
setlocal undolevels=-123456
setlocal varsofttabstop=
setlocal vartabstop=
setlocal virtualedit=
setlocal wincolor=
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let &fdl = &fdl
let s:l = 6 - ((5 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 6
normal! 018|
wincmd w
exe 'vert 1resize ' . ((&columns * 95 + 95) / 190)
exe 'vert 2resize ' . ((&columns * 94 + 95) / 190)
tabnext
edit Entity.cpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 95 + 95) / 190)
exe 'vert 2resize ' . ((&columns * 94 + 95) / 190)
argglobal
balt Entity.h
let s:cpo_save=&cpo
set cpo&vim
imap <buffer> <silent> <C-G>g <Plug>delimitMateJumpMany
imap <buffer> <S-BS> <Plug>delimitMateS-BS
imap <buffer> <C-H> <Plug>delimitMateBS
imap <buffer> <BS> <Plug>delimitMateBS
imap <buffer> <silent> g <Plug>delimitMateJumpMany
imap <buffer>  <Plug>delimitMateBS
imap <buffer> " <Plug>delimitMate"
imap <buffer> ' <Plug>delimitMate'
imap <buffer> ( <Plug>delimitMate(
imap <buffer> ) <Plug>delimitMate)
imap <buffer> [ <Plug>delimitMate[
imap <buffer> ] <Plug>delimitMate]
imap <buffer> ` <Plug>delimitMate`
imap <buffer> { <Plug>delimitMate{
imap <buffer> } <Plug>delimitMate}
let &cpo=s:cpo_save
unlet s:cpo_save
setlocal keymap=
setlocal noarabic
setlocal autoindent
setlocal backupcopy=
setlocal balloonexpr=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinscopedecls=public,protected,private
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,:///,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t
setlocal completefunc=
setlocal concealcursor=
setlocal conceallevel=0
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal cursorline
setlocal cursorlineopt=both
setlocal define=^\\s*#\\s*define
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal expandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fillchars=
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
set foldtext=SpaceVim#default#Customfoldtext()
setlocal foldtext=SpaceVim#default#Customfoldtext()
setlocal formatexpr=
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal formatoptions=croql
setlocal formatprg=
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=-1
setlocal include=^\\s*#\\s*include
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
set linebreak
setlocal linebreak
setlocal nolisp
setlocal lispoptions=
setlocal lispwords=
setlocal nolist
setlocal listchars=
setlocal makeencoding=
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal nomodeline
setlocal modifiable
setlocal nrformats=bin,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=ccomplete#Complete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal scrolloff=-1
setlocal shiftwidth=4
setlocal noshortname
setlocal showbreak=
setlocal sidescrolloff=-1
setlocal signcolumn=auto
setlocal smartindent
setlocal nosmoothscroll
setlocal softtabstop=4
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal spelloptions=
setlocal statusline=%#SpaceVim_statusline_a#%{SpaceVim#layers#core#statusline#mode(mode())}\ %{\ get(w:,\ \"winid\",\ winnr())\ }\ %#SpaceVim_statusline_a_SpaceVim_statusline_b#%#SpaceVim_statusline_b#%{\ &modified\ ?\ '\ *\ '\ :\ '\ -\ '}1.7k\ Entity.cpp\ %#SpaceVim_statusline_b_SpaceVim_statusline_c#%#SpaceVim_statusline_c#\ cpp\ %#SpaceVim_statusline_c_SpaceVim_statusline_b#%#SpaceVim_statusline_b#\ #\ s\ \ %#SpaceVim_statusline_b_SpaceVim_statusline_c#%#SpaceVim_statusline_b_SpaceVim_statusline_z#%#SpaceVim_statusline_z#%=%#SpaceVim_statusline_z#%{SpaceVim#layers#core#statusline#_current_tag()}%#SpaceVim_statusline_b_SpaceVim_statusline_z#%#SpaceVim_statusline_b#%{\"\ \"\ .\ g:_spacevim_statusline_fileformat\ .\ \"\ |\ \"\ .\ (&fenc!=\"\"?&fenc:&enc)\ .\ \"\ \"}%#SpaceVim_statusline_c_SpaceVim_statusline_b#%#SpaceVim_statusline_c#%{'\ '\ .\ join(map(getpos('.')[1:2],\ \"printf('%3d',\ v:val)\"),\ ':')\ .\ '\ '}%#SpaceVim_statusline_b_SpaceVim_statusline_c#%#SpaceVim_statusline_b#\ %P\ %#SpaceVim_statusline_c_SpaceVim_statusline
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=4
setlocal tagcase=
setlocal tagfunc=
setlocal tags=
setlocal termwinkey=
setlocal termwinscroll=10000
setlocal termwinsize=
setlocal textwidth=0
setlocal thesaurus=
setlocal thesaurusfunc=
setlocal undofile
setlocal undolevels=-123456
setlocal varsofttabstop=
setlocal vartabstop=
setlocal virtualedit=
setlocal wincolor=
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let &fdl = &fdl
let s:l = 65 - ((39 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 65
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("Entity.h", ":p")) | buffer Entity.h | else | edit Entity.h | endif
balt Entity.cpp
let s:cpo_save=&cpo
set cpo&vim
imap <buffer> <silent> <C-G>g <Plug>delimitMateJumpMany
imap <buffer> <S-BS> <Plug>delimitMateS-BS
imap <buffer> <C-H> <Plug>delimitMateBS
imap <buffer> <BS> <Plug>delimitMateBS
imap <buffer> <silent> g <Plug>delimitMateJumpMany
imap <buffer>  <Plug>delimitMateBS
imap <buffer> " <Plug>delimitMate"
imap <buffer> ' <Plug>delimitMate'
imap <buffer> ( <Plug>delimitMate(
imap <buffer> ) <Plug>delimitMate)
imap <buffer> [ <Plug>delimitMate[
imap <buffer> ] <Plug>delimitMate]
imap <buffer> ` <Plug>delimitMate`
imap <buffer> { <Plug>delimitMate{
imap <buffer> } <Plug>delimitMate}
let &cpo=s:cpo_save
unlet s:cpo_save
setlocal keymap=
setlocal noarabic
setlocal autoindent
setlocal backupcopy=
setlocal balloonexpr=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinscopedecls=public,protected,private
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,:///,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t
setlocal completefunc=
setlocal concealcursor=
setlocal conceallevel=0
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal cursorlineopt=both
setlocal define=^\\s*#\\s*define
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal expandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fillchars=
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
set foldtext=SpaceVim#default#Customfoldtext()
setlocal foldtext=SpaceVim#default#Customfoldtext()
setlocal formatexpr=
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal formatoptions=croql
setlocal formatprg=
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=-1
setlocal include=^\\s*#\\s*include
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},0),0],:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
set linebreak
setlocal linebreak
setlocal nolisp
setlocal lispoptions=
setlocal lispwords=
setlocal nolist
setlocal listchars=
setlocal makeencoding=
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal nomodeline
setlocal modifiable
setlocal nrformats=bin,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=ccomplete#Complete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal scrolloff=-1
setlocal shiftwidth=4
setlocal noshortname
setlocal showbreak=
setlocal sidescrolloff=-1
setlocal signcolumn=auto
setlocal smartindent
setlocal nosmoothscroll
setlocal softtabstop=4
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal spelloptions=
setlocal statusline=%#SpaceVim_statusline_ia#\ %{\ get(w:,\ \"winid\",\ winnr())\ }\ %#SpaceVim_statusline_ia_SpaceVim_statusline_b#%#SpaceVim_statusline_b#%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 36\ ?\ \"\"\ :\ (\"\ 866\ bytes\ Entity.h\ \ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 44\ ?\ \"\"\ :\ (\"\ \ cpp\ |\")}%{\ get(w:,\ \"winwidth\",\ 150)\ <\ 54\ ?\ \"\"\ :\ (\"\ \ #\ s\ \ \ |\")}%=|%{\"\ \"\ .\ &ff\ .\ \"|\"\ .\ (&fenc!=\"\"?&fenc:&enc)\ .\ \"\ \"}|\ %P\ 
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=4
setlocal tagcase=
setlocal tagfunc=
setlocal tags=
setlocal termwinkey=
setlocal termwinscroll=10000
setlocal termwinsize=
setlocal textwidth=0
setlocal thesaurus=
setlocal thesaurusfunc=
setlocal undofile
setlocal undolevels=-123456
setlocal varsofttabstop=
setlocal vartabstop=
setlocal virtualedit=
setlocal wincolor=
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let &fdl = &fdl
let s:l = 31 - ((24 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 31
normal! 030|
wincmd w
exe 'vert 1resize ' . ((&columns * 95 + 95) / 190)
exe 'vert 2resize ' . ((&columns * 94 + 95) / 190)
tabnext 5
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
set shortmess=filnxtToOScsF
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
nohlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
