nunki: server bidder seller 
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.o--------------------\n"
	@ ls *.o

server: auctionserver.h auctionserver.c
	gcc -o server.o -g -Wall auctionserver.c -lpthread -lsocket -lnsl -lresolv 

bidder: bidder.c bidder.h
	gcc -DBIDDERX=1 -o bidder1.o -g -Wall bidder.c -lpthread -lsocket -lnsl -lresolv
	gcc -DBIDDERX=2 -o bidder2.o -g -Wall bidder.c -lpthread -lsocket -lnsl -lresolv

seller: seller.h seller.c
	gcc -DSELLERX=1 -o seller1.o -g -Wall seller.c -lpthread -lsocket -lnsl -lresolv
	gcc -DSELLERX=2 -o seller2.o -g -Wall seller.c -lpthread -lsocket -lnsl -lresolv

u: server_u bidder_u seller_u 
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.o--------------------\n"
	@ ls *.o

server_u: auctionserver.h auctionserver.c
	gcc -o server.o -g -Wall auctionserver.c -lpthread  

bidder_u: bidder.c bidder.h
	gcc -DBIDDERX=1 -o bidder1.o -g -Wall bidder.c -lpthread
	gcc -DBIDDERX=2 -o bidder2.o -g -Wall bidder.c -lpthread

seller_u: seller.h seller.c
	gcc -DSELLERX=1 -o seller1.o -g -Wall seller.c -lpthread
	gcc -DSELLERX=2 -o seller2.o -g -Wall seller.c -lpthread


display: 
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.o--------------------\n"
	@ ls *.o
	@ echo "\n"

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
	
	
