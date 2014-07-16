# Contributing to APM Planner 2.0
We **<3 pull requests**, this is a short guide on how to write a succesful pull request for this project.

### Fork the project, then clone the repo

Fork https://help.github.com/articles/fork-a-repo

```
git clone git@github.com:YOUR-USERNAME/droneshare.git
```

make sure your environment is properly setup and navigate to the folder where you just cloned the repo

### Branch out of master

Always branch out of master unless you are targetting a specific branch for a release

```
git checkout -b descriptive_branch_name
```

and make sure you use a descriptive branch name

### Commit your changes

Always write descriptive commit messages and add a fixes or relates note to them with an issue number. The format is
```
<SUBSYSTEM>: <SHORT_DESCRIPTON> 

<LINE LONG_DESCRITPION>
```
APM Planner 2.0 Git Subsystems Names - https://docs.google.com/spreadsheets/d/1pvwlyny6EMIYt-viCxiLug-BUYC-jSYIWxciZIntUew/edit?usp=sharing

**Example:**

```
Mission Edit Widget: Add support for new command ROI

Added support for multiple ROIs in missions edit widget
and removed the unessecary mission lines

```
if you fix an issue, please add 'Fixes #<issue_number> to the short description as this will automatically mark the issue closed 

### Push your changes

Push changes to your repo and send a [pull request]

From here it's on us, we will reply with suggestions changes or improvements, we will try to reply as fast as we can.
