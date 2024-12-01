import os

"""
    As the creator of Advent of Code apparently does not want to have the original puzzle inputs published (they are different for everyone), 
    I also have to remove them from git history (I did not know this initially) after having moved them with migrate_input_files.py.

    After running 1.) migrate_input_files.py and 2.) rewrite_history.py, the inputs for each day-nn will now be placed into input/day-nn.txt 
    while being removed from git's history and while not being tracked by git anymore (apart from the example inputs; cf. .gitignore). 
"""

if __name__ == "__main__":
    root = os.getcwd()

    y_n = input("Remove input files from git history? (y/n) ")
    if y_n.lower() != "y":
        print("Aborted.")
        exit(1)

    for dir in os.listdir(root):
        if "day-" not in dir:
            continue
        if dir == "day-xy":
            continue
        print(dir)
        
        cmd = f'git filter-branch -f --index-filter "git rm -rf --cached --ignore-unmatch {dir}/input.txt" HEAD' # It's dangerous...
        print("Running command: '" + cmd + "'")
        os.system(cmd)   