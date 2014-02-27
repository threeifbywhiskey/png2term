png2term
========

**png2term** is a pair of programs that convert PNG images into approximations suitable for display in 256-color terminals. The C program uses a color adaptation algorithm adopted from [desert256.vim](http://www.vim.org/scripts/script.php?script_id=1243) while the Ruby one uses a much simpler base-6 approach to color selection. The C version is significantly faster, more accurate, and outputs "optimized" ANSI escape codes, but the Ruby program is pleasantly concise. Both use Unicode box drawing characters (▀ UPPER HALF BLOCK U+2580 and ▄ LOWER HALF BLOCK U+2584) to place two pixels per row, thus retaining better dimensional proximity to the original image.
