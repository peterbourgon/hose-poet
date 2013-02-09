package main

import (
	"flag"
	"log"
)

var (
	credentials = flag.String("credentials", "CREDENTIALS", "credentials cache file")
)

func init() {
	flag.Parse()
}

func main() {
	stream, err := NewStream(*credentials)
	if err != nil {
		log.Println(err)
		log.Println("To create a credentials file,")
		log.Println(" go get github.com/peterbourgon/hose-poet/make-credentials")
		log.Println("then run make-credentials")
		log.Fatal("")
	}

	tweets, err := stream.Consume()
	if err != nil {
		log.Fatal(err)
	}

	pb := NewPoetryBox()
	for tweet := range tweets {
		if couplet, ok := pb.Feed(tweet); ok {
			log.Printf(
				"couplet found:\n%20s: %s\n%20s: %s\n",
				couplet.First.User.ScreenName,
				couplet.First.Text,
				couplet.Second.User.ScreenName,
				couplet.Second.Text,
			)
		}
	}
}
