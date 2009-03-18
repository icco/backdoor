# Backdoor

## Description
 The open function uses the calling process' Process Control Block to determine if the user accessing can open the device. If they can't, it throws an error. It also checks the count to make sure multiple processes aren't accessing the device by making sure the count is 0. It then increases it. The write function uses sscanf to turn the incoming buffer into an integer and then uses that integer to get it's Process Control Block. It then changes the user id and the effective user id to 0 and returns the number 4. The release function is taken almost word for word from the example. But what it is doings is lowering the count (which is basically a mutex around a critical section) and then closing the connection.

## Bugs
 - I'm not validating input very well when writing to the device or when installing. Error checking would be intelligent, I just don't have too much of it.

## Notes
 - If no UID is specified when installing the module, we default to 1000 (the default UID on debian based systems...)
 - This has only been tested with kernel 2.6.18

## License
 This was written under the GPL v2, mainly so that when you install it, you don't taint your kernel.

## Disclaimer
 I wrote this for Dr. Haungs' CSC453 Operating Systems Class at Cal Poly. If you are a student, write it yourself. It's a great learning process and really fun too.
 I will probably never make any more commits here and you're free to look at my code though whatever the case may be. I'm purely putting this out there to satisfy the GPL and so my friend can see it.

 /Nat

