package main

import (
	"flag"
	"log"
)

var (
	credentials = flag.String("credentials", "CREDENTIALS", "credentials cache file")
	rhymeBin    = flag.String("rhyme.bin", "./rhyme-0.9/rhyme.bash", "location of 'rhyme.bash' script")
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

	rh := NewRhyme(*rhymeBin)
	st := NewStore(rh)
	for tweet := range tweets {
		if match, ok := st.Feed(tweet); ok {
			log.Printf("\n\n")
			for _, t := range match {
				log.Printf("  • %20d %s: %s", t.ID, t.User.ScreenName, t.Text)
			}
			log.Printf("\n\n")
		}
	}
}
