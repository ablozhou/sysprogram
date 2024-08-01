
# Give some Transactions with it's timestamp and instructions,
# check whether they are accept or reject
# Authors: Haihan Zhou / 2097902, Weisiang Hong / 2098012, Nigar Sultana Kana, ID : 2098363
# Date: 2024-07-10

'''
# r_ts(A), w_ts(A),r_ts(B), w_ts(B),TS1=10, TS2=20
# input file format:

A='A';B='B';T25='T25';T26='T26'
RTS[A] =0; WTS[A] = 0; RTS[B] = 0; WTS[B] = 0
TS[T25]=25; TS[T26]=26
read(B) |
        | read(B)
        | B=B-50
        | write(B)
read(A) |
        | read(A)
display(A + B)|
        | A:=A+50
        | write(A)
        | display(A+B)
# ---
Q='Q';T27='T27';T28='T28'
RTS[Q] =0; WTS[Q] = 0;
TS[T27]=27; TS[T28]=28

read(Q) |
        | write(Q)
write(Q)|

'''
import sys
import os
import re
import csv

from init import *

# A='A';B='B'
# T25='T25';T26='T26'
# RTS={A:0,B:0}
# WTS={A:0,B:0}
# #RTS[A] =0; WTS[A] = 0; RTS[B] = 0; WTS[B] = 0
# TS={T25:25,T26:26}
#TS[T25]=25; TS[T26]=26
#data_pattern = "|".join(data)
#pattern = rf'(read|write)\(({data_pattern})\)'
def parse_transation(file_path):
    trans =[]#'T25','T26'
    #ts_init_time = [25,26]
    pattern = r'(read|write)\((\w+)\)'
    data=[]

    rts_init_time=0
    wts_init_time=0

    rts={} # R-TS['A']=0
    wts={}
    ts={}
    trans={}
    deadlock=False
    
    results = []
    
    file_name = os.path.basename(file_path)
    directory = os.path.dirname(file_path)
    stem, suffix = os.path.splitext(file_name)
    
    result_name=stem+"_r.txt"
    conclusion=""
    
    if not file_path or not os.path.exists(file_path):
        printf(f"error open {file_path}")
        return
    
    
    with open(file_path) as tf: 
        reader = csv.DictReader(tf, delimiter='|',skipinitialspace=True,quoting=csv.QUOTE_NONE)
        
        trans = reader.fieldnames
        
        ts_init_time = next(reader)
        print(f"ts_init_time:{ts_init_time}")
        
        for i,t in enumerate(trans):
            ts[t] = int(ts_init_time[t])
            print(f"init ts[{t}]={ts_init_time[t]}")
           
        for i,row in enumerate(reader):
            
            print(f'\n{i}: {row}')
            result = []
            for t in trans:
                
                #if row[t].startswith("read("): #read
                match = re.match(pattern, row[t],re.IGNORECASE)
                if match:
                    function_name = match.group(1)
                    dataitem = match.group(2)
                    if dataitem not in data:
                        data.append(dataitem)
                        rts[dataitem]=rts_init_time
                        wts[dataitem]=wts_init_time
                        print(f"init rts[{dataitem}]={rts_init_time}")
                        print(f"init wts[{dataitem}]={wts_init_time}")
                        result.append(f"rts_{dataitem}:{rts_init_time}|wts_{dataitem}:{wts_init_time}")
                    if function_name == "read":
                        if ts[t]>=wts[dataitem]: # TS(Ti) ≥ W-timestamp(Q) accept
                            rts[dataitem]=ts[t]
                            print(f"{i}:Reading of {t}:{row[t]} successful. rts[{dataitem}]={ts[t]}")
                            result.append(f"{i}:rts_{dataitem}:{rts[dataitem]}|wts_{dataitem}:{wts[dataitem]}")
                        else:
                            result.append(f"{i}:ts[{t}]={ts[t]},rts_{dataitem}:{rts[dataitem]}|wts_{dataitem}:{wts[dataitem]}")
                            conclusion = f"{i}:{t}:{row[t]} reject read, {t} need for rollback.\n"
                            #print(conclusion)
                            
                            deadlock = True
                            break
                    elif function_name == "write":
                        if ts[t]>=rts[dataitem] and ts[t]>=wts[dataitem]:
                            wts[dataitem]=ts[t]
                            print(f"{i}:Writing of {t}:{row[t]} successful. wts[{dataitem}]={ts[t]}")
                            result.append(f"{i}:rts_{dataitem}:{rts[dataitem]}|wts_{dataitem}:{wts[dataitem]}")
                        # if ts[t]>=wts[dataitem]:
                        #     wts[dataitem]=ts[t]
                        #     print(f"wts[{dataitem}]={ts[t]}")
                        #     result.append(f"rts_{dataitem}:{rts[dataitem]}|wts_{dataitem}:{wts[dataitem]}")

                        else:
                            result.append(f"{i}:ts[{t}]={ts[t]},rts_{dataitem}:{rts[dataitem]}|wts_{dataitem}:{wts[dataitem]}")
                            conclusion = f"{i}:{t}:{row[t]} reject write, {t} need for rollback.\n"
                            #print(conclusion)
                            
                            deadlock = True
                             
                            break
                else:
                    if row[t]:
                        print(f"{row[t]} ignored")
                    
            #f.read()
            results.append(result)
        if deadlock:
            print(conclusion)
        else:
            conclusion = f"\nThere is no conflict in {trans}."
            print(conclusion)
        
        print(f"results:{results}")
        with open(result_name, "w") as rwtsf:
            rh = ["R-TS("+k+")" for k in rts.keys()]
            wh = ["W-TS("+k+")" for k in wts.keys()]
            
            fieldnames = [ val for pairs in zip(rh,wh) for val in pairs]
            #writer = csv.DictWriter(rwtsf, delimiter='|',fieldnames=fieldnames,skipinitialspace=True,quoting=csv.QUOTE_NONE)
            #writer.writerows(results)
            rwtsf.write(f"{fieldnames}\n")
            for i in results:
                if i:
                    for j in i:
                        rwtsf.write(f"{j}\n")
            
            rwtsf.write(conclusion)
        
        
def main():
    # check args number
    if len(sys.argv) < 2:
        print(f"Usage: python {sys.argv[0]} <transaction.csv>")
        sys.exit(1)

    file_path = sys.argv[1]
    print(f"file: {file_path}")
    parse_transation(file_path)

if __name__ == "__main__":
    main()     
        