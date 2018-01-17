import os
import re
print (os.getcwd())
# testfile = input()
# os.system("Test1.exe "+testfile)
ifprintln = False
for fn in os.listdir('.'):
    if fn.endswith(".txt") and len(re.findall(r"\_",fn))==1 and fn.startswith("test"):
        print(fn)
        
        os.system("echo "+fn+" | ./a.out")
        for i in range(1,10):
            infilename = fn.split(".")[0]+"_"+str(i)+".txt"
            outfilename = fn.split(".")[0]+"_"+str(i)+"_out.txt"
            print(outfilename)
            if os.path.exists(infilename):
                os.system("java -jar Mars4_4.jar nc asm.txt > tempout.txt < "+infilename)
                if not ifprintln:
                    with open(outfilename,'r') as outfile:
                        with open("newtempout.txt","w") as newoutfile:
                            newoutfile.write(re.sub(r'\n',"",outfile.read()))
                    os.system("diff -b -B tempout.txt newtempout.txt")
                else:
                    os.system("diff -b -B tempout.txt "+outfilename)
                print(outfilename)
            else:
                break
		#os.system("pause")
os.system("pause")
