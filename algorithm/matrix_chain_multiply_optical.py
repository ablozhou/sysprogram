
# compute the optimal cost by using a tabular, bottom-up approach
# Book Algorithm Corner 4th. ch.14
# p: input sequence dimensions array of matrix Ai. p=[p0,p1,...pn]
# A1[30,35],A2[35,15],A3[15,5],A4[5,10],A5[10,20],A6[20,25]
# p=[30,35,15,5,10,20,25]
# n: length of p, for i = 1...n, Ai has dimension p_{i-1}xp_i
# The procedure uses an auxiliary table m[1:n,1:n] to store costs,
# s[1:n-1, 2:n] to store index k achieved the optimal m[i,j].
# The table will help in constructing an optimal solution.
def calculate(p, n):
    m =[[0]*n for i in range(n)] # dimension 0 is not use
    s =[[0]*n for i in range(n)]
    
    max_int = float('inf')
    
    for l in range(2,n): # l: length of the chain, from 2 matrices to entire chain, which is n-1
        for i in range(1,n-l+1): # begin from 1, end is 
            j = i + l - 1  # end
            m[i][j] = max_int
            for k in range(i,j):
                q = m[i][k]+m[k+1][j]+p[i-1]*p[k]*p[j] 
                if q < m[i][j]:
                    m[i][j] = q
                    s[i][j] = k

    return m,s
def print_optimal_parens(A,s,i,j):
    if i == j:
        #print(f"A{i}",end='')
        print(f"{A[i-1]}",end='')
    else:
        print(f"(",end='')
    
        print_optimal_parens(A,s,i,s[i][j])
        print_optimal_parens(A,s,s[i][j]+1,j)
        
        print(f")",end='')
        
def chain_multiply(A,s,i,j):
    if i == j:
        print(f"{A[i-1]}",end='')
        return A[i-1]
    else:
        C=chain_multiply(A,s,i,s[i][j])
        D=chain_multiply(A,s,s[i][j]+1,j)
        return C*D
        
def main():
    ## A1[30,35],A2[35,15],A3[15,5],A4[5,10],A5[10,20],A6[20,25]
    A=["A1", "A2", "A3", "A4", "A5", "A6"]
    p=[30,35,15,5,10,20,25]
    size = len(A)
    m,s = calculate(p,size+1)
    
    print(f"m:{m}")
    print(f"s:{s}")
    print(f"cost:{m[1][size]}")
    print_optimal_parens(A,s,1,size)
    print("")
    
if __name__ == "__main__":
    main()