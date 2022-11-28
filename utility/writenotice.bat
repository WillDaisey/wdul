@echo off

rem This batch file writes or updates a comment at the beginning of each source file.
rem Requires Will Daisey's copynotice tool v1.X.X. See https://github.com/WillDaisey/copynotice.

copynotice /verbose /dir ".." "out" /dir "..\include\wdul" "out\include\wdul" /notef "notice.txt" /ext "cpp" /ext "hpp" /replace
