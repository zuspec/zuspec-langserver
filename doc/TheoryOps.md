
Startup Operations
==================
- When first started, the LS should perform config and source discovery
- Simplest is:
  - Find all PSS files in the workspace directories


Processing relies on distinguishing between on-disk files that are not
being edited ('closed' files) and files whose current content is 'live'
in the editor ('opened' files).

Two sets of files, tracked by URI:
- All PSS files
- Opened PSS files

Files are further categorized as either having or not having a pss_top 
declaration.
Note: Need to consider 'pss_top' declarations as unique. It's common
to have many, and we don't want to always trigger the 'duplicate' error.
Note: Could choose to not report duplicate types on the theory that
these are desired.


Note: must be able to periodically run the 'discover files' process and
incorporate newly-discovered files into the indexed set

- Need to have an AST reflecting the on-disk version of all PSS files
  at all times.

- Need to have an AST reflecting the 'live' version for all 'opened'
  files at all times.



SourceManager
  - list-of: Workspace Paths
  - set-of: Discovered files
    - Last-known-modified timestamp
    - 
  - set-of: Currently-open files


# Searching through source info
- 


# Approach
- Focus, first, on providing per-file information
  - Symbols
  - Hover

# Task queue
- Tasks are dispatched from a queue by an 'idle' callback
- Having a task queue enables us to manage dependencies 
- Tasks can be multi-step

## Initialize workspace
- Discover source files
- Parse each file to an AST
- 