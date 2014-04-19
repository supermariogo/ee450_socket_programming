nunki: port server bidder seller 
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.o--------------------\n"
	@ ls *.o
port:
	gcc port.c -o port.o -DSERVERHOST=NUNKI -g -c -Wall
server: 
	gcc auctionserver.c port.o -o server.o -g -Wall -lpthread -lsocket -lnsl -lresolv 

bidder: 
	gcc bidder.c port.o -o bidder1.o -DBIDDERX=1 -DSERVERHOST=NUNKI -g -Wall -lpthread -lsocket -lnsl -lresolv
	gcc bidder.c port.o -o bidder2.o -DBIDDERX=2 -DSERVERHOST=NUNKI -g -Wall -lpthread -lsocket -lnsl -lresolv
                                     
seller:                              
	gcc seller.c port.o -o seller1.o -DSELLERX=1 -DSERVERHOST=NUNKI -g -Wall -lpthread -lsocket -lnsl -lresolv
	gcc seller.c port.o -o seller2.o -DSELLERX=2 -DSERVERHOST=NUNKI -g -Wall -lpthread -lsocket -lnsl -lresolv

u: port_u server_u bidder_u seller_u 
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.o--------------------\n"
	@ ls *.o
port_u: 
	gcc port.c -o port.o -DSERVERHOST=LOCALHOST -g -c -Wall

server_u: 
	gcc auctionserver.c port.o -o server.o -g -Wall -lpthread  

bidder_u: 
	gcc bidder.c  port.o -o bidder1.o -DBIDDERX=1 -DSERVERHOST=LOCALHOST -g -Wall -lpthread
	gcc bidder.c  port.o -o bidder2.o -DBIDDERX=2 -DSERVERHOST=LOCALHOST -g -Wall -lpthread

seller_u: 
	gcc seller.c  port.o -o seller1.o -DSELLERX=1 -DSERVERHOST=LOCALHOST -g -Wall -lpthread
	gcc seller.c  port.o -o seller2.o -DSELLERX=2 -DSERVERHOST=LOCALHOST -g -Wall -lpthread

clean:
	rm -f *.o *~ *.out *.so

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
	
	
