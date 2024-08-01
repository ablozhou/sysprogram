
def heap_sort(arr):
  # Assign the length of the array to a variable
  arr_len = len(arr)

  # Loop through the length of the array to ensure a depth wise...
  for i in range(arr_len//2-1,-1,-1):
      # Create maxheaps from the array
      heapify(arr, arr_len, i)

  print("===max heap ok===")
  
  for i in range(arr_len-1, 0, -1):
      # Swapping the first element with the current element
      arr[i], arr[0] = arr[0], arr[i]
      heapify(arr, i, 0)

  return arr
def heapify(arr, n, i):
  print(f'{i}:{arr}')
  # Initialize largest as root
  largest = i
  # left child = 2*i + 1
  l = 2 * i + 1
  # right child = 2*i + 2
  r = 2 * i + 2

  # If left child is larger than root
  if l < n and arr[i] < arr[l]:
      largest = l

  # If right child is larger than largest so far
  if r < n and arr[largest] < arr[r]:
      largest = r

  # If largest is not root
  if largest != i:
      # swap with root
      arr[i], arr[largest] = arr[largest], arr[i]
      # Recursively heapify the affected sub-tree
      heapify(arr, n, largest)

arr=[9,7,3,5,11,12,31]
print(heap_sort(arr))
