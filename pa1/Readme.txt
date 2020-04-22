Programming Assignment 1: DNS Client
18-4-2020
By Roy Pasternak (204219273) and Doron Rachamim (205989957)

The DNS Client 'nsclient.exe' is a program similar to the windows nslookup command.
The program expect to get one input argumnet which is the IP of the DNS server.
Then the user should input the domain name he/she want to look for their ip address.
A query contains the domain name will be sent to the DNS server given and the program will wait for an answer for
a maximum of two seconds. If an answer recived in that time frame the program will print the recived ip address and 
fill in the hostnet strcut with the address and the data len (only those two fields).

Answer parsing:
As for the answer parsing, we support the following features:
1. Header parsing: id, qr bit and error code.
2. Assume one query recived - read its type and class
3. Search in all answers recived for the first answer has same type and class, then parse the data_len and ip.

Constarints and limitaion:
1. The domain name must be legal (any ilegal name will result in "ERROR: BAD NAME")
2. Two seconds limitation between send and recv, if no answer recived in that time frame - "TIMEOUT".
3. The answer recived must has the same ID as the query sent, otherwise - "TIMEOUT".
4. The nsclient supports only one query at a time. Therefore we assume that the answer will always contain
   only ONE query. If we recived more then one query a message will be printed to the screen:
	"Supports no more than 1 query (%d queries recived). Try again..."
5. The nsclient check for errors in the ERRORCODE bits in the answer and prints the following error:
6. If no answer was found with the same type and class as the query -> an error message will follow: 
   "MESSAGE: NO ANSWER FOUND WITH CORRESPONDING TYPE AND CLASS OF THE QUERY SENT. TRY AGAIN..."
7. Any SystemCall error will result in perror() : sendto, recvfrom and select.

Code structre:
The project containts the following modules:
1. main - handles agrument parsing and user input, then calling the dnsQuery method.
2. dns_protocol - implemntation of the dnsQuery including all the socket relate methods (send and recv), plus handles
                  the query parsing.
3. answer_parser - this module parses the answer recived and return a "strcut answer" contains the ip address and its data length.
4. utils - contains some general methods and defines.