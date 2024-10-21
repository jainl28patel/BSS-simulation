# BSS Protocol Simulation

## Compile and Run on Linux
* Place input in the `input.txt` file.
* Run the below to execute the program

```base
chmod +x run.sh
./run.sh
```

* Output will be printed in `./results/output.txt`

## Compile and Run on Windows


## Sample Input and Outputs

### Input-1
```
begin process p1
send m1
end process p1

begin process p2
recv_B p1 m1
send m2
end process p2

begin process p3
recv_B p2 m2
recv_B p1 m1
end process p3
```

### Input-2
```
begin process p1
send m1
recv_B p2 m2
end process p1

begin process p2
recv_B p1 m1
send m2
send m3
end process p2

begin process p3
recv_B p2 m3
recv_B p2 m2
recv_B p1 m1
end process p3
```

### Output-1
```
begin process p1
	send m1 (100)
end process p1

begin process p2
	recv_B p1 m1 (000)
	recv_A p1 m1 (100)
	send m2 (110)
end process p2

begin process p3
	recv_B p2 m2 (000)
	recv_B p1 m1 (000)
	recv_A p1 m1 (100)
	recv_A p2 m2 (110)
end process p3
```

### Output-2
```
begin process p1
	send m1 (100)
	recv_B p2 m2 (100)
	recv_A p2 m2 (110)
end process p1

begin process p2
	recv_B p1 m1 (000)
	recv_A p1 m1 (100)
	send m2 (110)
	send m3 (120)
end process p2

begin process p3
	recv_B p2 m3 (000)
	recv_B p2 m2 (000)
	recv_B p1 m1 (000)
	recv_A p1 m1 (100)
	recv_A p2 m2 (110)
	recv_A p2 m3 (120)
end process p3
```