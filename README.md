# mercury_autonomy
BT autonomy system for the Mercury stack


# Dependency Management: BehaviorTree.CPP
The mercury_autonomy package vendors the BehaviorTree.CPP library using Git Subtree. This enables us to have bespoke revision of the library while allowing us to pull upstream changes for maintinance and new features.

The library source is located at: dependencies/BehaviorTreeCPP

## Working with the Subtree

1. Setup the Upstream Remote
Before pulling updates, you must define the official repository as a remote on your local machine. You only need to do this once.

```bash
git remote add btCPP_upstream https://github.com/BehaviorTree/BehaviorTree.CPP.git
```

2. Pulling Upstream Updates
To bring in new features or fixes from the official library, use the following command. Note: You must include the --squash flag to keep our commit history clean.

```bash
git fetch btCPP_upstream master
git subtree pull --prefix=dependencies/BehaviorTree.CPP bt_upstream master --squash
```

3. Local Modifications
You can edit the code within dependencies/BehaviorTree.CPP directly.

Treat these files like any other part of the Mercury codebase.

Commit and push changes to the mercury_autonomy repository normally.

Note that during a future subtree pull, Git will attempt to merge your local changes with the upstream updates. Resolve any conflicts as you would in a standard merge.


# BT Assistant

Currently in progress, as a successor to the BT assistant present in riptide_autonomy.

BT Assistant is a python script that automates the creation and registration of custom nodes.
 