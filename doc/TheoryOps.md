
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

Note: must be able to periodically run the 'discover files' process and
incorporate newly-discovered files into the indexed set

- Need to have an AST reflecting the on-disk version of all PSS files
  at all times.

- Need to have an AST reflecting the 'live' version for all 'opened'
  files at all times.


Note: Need to consider 'pss_top' declarations as unique. It's common
to have many, and we don't want to always trigger the 'duplicate' error.
