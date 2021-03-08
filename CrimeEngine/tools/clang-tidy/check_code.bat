@echo off
@echo "Running static code analysis..."
@echo "Creating compilation database for clang-tidy..."
@py create_compile_database.py
@echo "Database created!"
@echo "Running clang-tidy on code..."
@if exist "clang-tidy-output" del /F "clang-tidy-output"
@py run-clang-tidy.py -extra-arg= >> "clang-tidy-output"
@echo "Done checking code!"
@echo "Converting clang-tidy-output to junit.xml file..."
@type clang-tidy-output | py clang-tidy-to-junit.py ./ >junit.xml
@echo "Done converting code, showing code analysis results..."
@rem Check inside the test results for the number of errors since we can't add a post-build step in jenkins 
@rem we have to check before hand how many errors we have.
for /F "tokens=1,8,9 delims=<= " %%a in (junit.xml) do (
   if "%%a" equ "testsuites" (
    if "%%b" equ "errors" (
        SET NUM_ERRORS=%%c
    )
   )
)
@rem Remove quotation marks from the number of errors variable
for /F "delims=" %%I in (%NUM_ERRORS%) do SET NUM_ERRORS=%%I

@rem Check if we have any errors inside our test result if so set the build to unstable.
@echo Number of errors= %NUM_ERRORS%
if %NUM_ERRORS% GTR 0 (
    @echo Build unstable...
    SET BUILD_STABLE=0
) ELSE (
    @echo Build is stable...
    SET BUILD_STABLE=1
)
