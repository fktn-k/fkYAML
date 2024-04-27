The following changes have been made to the code with respect to <https://github.com/edlund/amalgamate/commit/c91f07eea1133aa184f652b8f1398eaf03586208>

* Resolved inspection results from Codacy:
    * removed trailing whitespaces
    * removed unused import `sys`
    * removed unused local variable `actual_path` in the generate() method of class Amalgamation
    * fixed issues which are against the style conventions in PEP 8
* Forced newline codes in the amalgamated file to be LF to avoid huge diff (LF->CRLF) after running amalgamation on Windows.
