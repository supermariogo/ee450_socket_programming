nunki: port server bidder seller 
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.o--------------------\n"
	@ ls *.o
port:
	gcc -g -DSERVERHOST=NUNKI -c -Wall -o port.so port.c
server: 
	gcc -o server.o -g -Wall auctionserver.c -lpthread -lsocket -lnsl -lresolv 

bidder: 
	gcc -DBIDDERX=1 -DSERVERHOST=NUNKI -o bidder1.o -g -Wall bidder.c port.so -lpthread -lsocket -lnsl -lresolv
	gcc -DBIDDERX=2 -DSERVERHOST=NUNKI -o bidder2.o -g -Wall bidder.c port.so -lpthread -lsocket -lnsl -lresolv

seller: 
	gcc -DSELLERX=1 -DSERVERHOST=NUNKI -o seller1.o -g -Wall seller.c port.so -lpthread -lsocket -lnsl -lresolv
	gcc -DSELLERX=2 -DSERVERHOST=NUNKI -o seller2.o -g -Wall seller.c port.so -lpthread -lsocket -lnsl -lresolv

u: port_u server_u bidder_u seller_u 
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.o--------------------\n"
	@ ls *.o
port_u: 
	gcc -g -DSERVERHOST=LOCALHOST -c -Wall  -fPIC -shared port.c -o port.so

server_u: 
	gcc -o server.o -g -Wall auctionserver.c -L. port.so -lpthread  

bidder_u: 
	gcc -DBIDDERX=1 -DSERVERHOST=LOCALHOST -o bidder1.o -g -Wall bidder.c -L. port.so -lpthread
	gcc -DBIDDERX=2 -DSERVERHOST=LOCALHOST -o bidder2.o -g -Wall bidder.c -L. port.so -lpthread

seller_u: 
	gcc -DSELLERX=1 -DSERVERHOST=LOCALHOST -o seller1.o -g -Wall seller.c -L. port.so -lpthread
	gcc -DSELLERX=2	-DSERVERHOST=LOCALHOST -o seller2.o -g -Wall seller.c -L. port.so -lpthread



clean:
	rm -f *.o *~ *.out

cat1:
	cat Registration.txt
	@echo "----------------------------------------------------"
	@cat bidderPass1.txt
	@cat bidderPass2.txt
	@cat sellerPass1.txt
	@cat sellerPass2.txt
cat2:
	cat broadcastList.txt
	@echo "------------------------seller1 sell----------------------------"
	cat itemList1.txt
	@echo "------------------------seller2 sell----------------------------"
	cat itemList2.txt
	@echo "------------------------bidder1 bidding----------------------------"
	cat bidding1.txt
	@echo "------------------------bidder2 bidding----------------------------"
	cat bidding2.txt
	
	
