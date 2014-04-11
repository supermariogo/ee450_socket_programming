default: server bidder seller 
	mv bidder.out bidder1.out
	cp bidder1.out bidder2.out
	mv seller.out seller1.out
	cp seller1.out seller2.out
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.out--------------------\n"
	@ ls *.out
	@ echo "\n"

server: auctionserver.h auctionserver.cpp
	g++ -o server.out -g -Wall -L/opt/lib auctionserver.cpp -lpthread -lsocket -lnsl -lresolv

bidder: bidder.cpp bidder.h
	g++ -o bidder.out -g -Wall bidder.cpp -lpthread -lsocket -lnsl -lresolv 

seller: seller.h seller.cpp
	g++ -o seller.out -g -Wall seller.cpp -lpthread -lsocket -lnsl -lresolv 

main.o:	main.h main.cpp
	g++ -g -c -Wall main.cpp -lpthread

display: 
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.out--------------------\n"
	@ ls *.out
	@ echo "\n"

clean:
	rm -f *.o *~

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
	
	
