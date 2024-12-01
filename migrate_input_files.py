import os

# Move the input files for each day-nn from "day-nn/input.txt" to "input/day-nn.txt"

if __name__ == "__main__":
    root = os.getcwd()

    y_n = input("Migrate input files from directory" + root + "? (y/n) ")
    if y_n.lower() != "y":
        print("Aborted.")
        exit(1)

    for dir in os.listdir(root):
        if "day-" not in dir:
            continue
        if dir == "day-xy":
            continue
        print(dir)
        
        for fname in os.listdir(os.path.join(root, dir)):
            if "input" not in fname or (".txt" not in fname) : 
                continue

            example_str = ""
            if "example" in fname: 
                example_str = "-example"

            new_name = dir + example_str + ".txt"
            new_path = os.path.join(root, "input", new_name)
            old_path = os.path.join(root, dir, fname)

            os.rename(old_path, new_path)
            print(f'moved file from {old_path} to {new_path}')

        print()