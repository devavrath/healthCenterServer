Author: Devavrath Subramanyam	
-----------------------------------------

1. I have coded 5 C files namely healthcenterserver.c, doctor1.c, doctor2.c, patient1.c and patient2.c.
2. Each C file performs a given set of functions which is decribed below.

The socket programming project simulates an online medical appointment system using a combination of TCP and
UDP sockets. In the execution of the project, we have 3 phases namely:

Phase 1: Authentication of the patient by the server:
The patient connects to the healthcenterserver via a TCP connection, sends his credentials via a TCP connection.
The server receives these credentials, authenticates the credentials and sends either a "SUCCESS/FAILURE" message 
to the patient.

Phase 2: Appointment Booking:
Once the patient is authenticated, the patient will request for a set of available appointments from the server.
The health center server sends a list of appointments available to the patient. The patient has to enter an available
valid index. I have validated the entered index on the patient side itself.
For eg: if the available slots are 1, 2, 3 and 4 and if the patient enters "5", I flag an error and throw a message onto 
the patient, displaying "Incorrect Index!! Please Enter the index again:" and give the client a second chance to enter the
right index. If the client enters a wrong index again, I provide an "Error" message and exit the program, thus closing the TCP
connection. I also send the wrongly entered index to the server, which promptly rejects the patient's wrong request.
If the patient enters a correct index, the server returns the doctor name and port number on which the doctor is present.
Next the TCP connection is closed.

Phase 3: Appointment Confirmation:
The doctors doctor1 and doctor2 are listening on static UDP ports. Once the patient receives the doctor name and port number 
after phase 2, the patient sets up a UDP connection with the doctor based on the port number he received. The patient also sends
the insurance name he is on to the doctor via the same UDP socket. The doctor receives the insurance name and checks the price
for the corresponding insurance name and sends the price back to the patient.
-----------------------------------------

Code Files:  
1. server.c: Performs patient authentication, sends the list of available appointments to the patient and then provides the name
and port number of the doctor.

2. doctor1.c : Doctor 1 is listening on port 41849. Once he receives a UDP request from a patient, he checks the insurance type and
based on the insurance type, he sends the cost to the patient over the same UDP socket. 

3. doctor2.c : Doctor 2 is listening on port 42849. Once he receives a UDP request from a patient, he checks the insurance type and
based on the insurance type, he sends the cost to the patient over the same UDP socket.

4. patient1.c : Patient 1 connects with the server over a TCP connection, performs authentication and after authentication receives
list of available appointments and upon selection of one, connects to the concurrent doctor, and gets the estimated price for the appointment.

5. patient2.c : Patient 2 connects with the server over a TCP connection, performs authentication and after authentication receives
list of available appointments and upon selection of one, connects to the concurrent doctor, and gets the estimated price for the appointment.
-----------------------------------------

Compilation and Execution Commands:-
1. Please open a terminal and run the following command - gcc healthcenterserver.c -o healthcenterserver -lsocket -lnsl -lresolv
2. Run the command in a terminal - gcc doctor1.c -o doctor1 -lsocket -lnsl -lresolv
3. Run the command in a terminal - gcc doctor2.c -o doctor2 -lsocket -lnsl -lresolv
4. Run the command in a terminal - gcc patient1.c -o patient1 -lsocket -lnsl -lresolv
5. Run the command in a terminal - gcc patient2.c -o patient2 -lsocket -lnsl -lresolv
6. Execute all the executable starting from the server
-----------------------------------------

