@echo off

FOR /R Source %%F IN (*.cpp, *.h) DO (
    clang-format -i %%F
    echo %%F
)
PAUSE