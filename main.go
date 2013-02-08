package main

import (
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"strings"

	"github.com/mrjones/oauth"
)

var (
	credentials    = flag.String("credentials", "CREDENTIALS", "credentials cache file")
	consumerKey    = flag.String("consumer.key", "", "consumer key")
	consumerSecret = flag.String("consumer.secret", "", "consumer secret")
	accessToken    = flag.String("access.token", "", "access token")
	accessSecret   = flag.String("access.secret", "", "access token secret")
)

func init() {
	flag.Parse()
}

func main() {
	if buf, err := ioutil.ReadFile(*credentials); err == nil {
		if toks := strings.Split(string(buf), "\n"); len(toks) >= 4 {
			*consumerKey = toks[0]
			*consumerSecret = toks[1]
			*accessToken = toks[2]
			*accessSecret = toks[3]
		}
	}

	if *consumerKey == "" || *consumerSecret == "" {
		log.Fatal("-consumer.key and -consumer.secret required")
	}

	c := oauth.NewConsumer(
		*consumerKey,
		*consumerSecret,
		oauth.ServiceProvider{
			RequestTokenUrl:   "http://api.twitter.com/oauth/request_token",
			AuthorizeTokenUrl: "https://api.twitter.com/oauth/authorize",
			AccessTokenUrl:    "https://api.twitter.com/oauth/access_token",
		},
	)

	if *accessToken == "" || *accessSecret == "" {
		requestToken, url, err := c.GetRequestTokenAndUrl("oob")
		if err != nil {
			log.Fatal(err)
		}

		fmt.Println("(1) Go to: " + url)
		fmt.Println("(2) Grant access, you should get back a verification code.")
		fmt.Println("(3) Enter that verification code here: ")
		var verificationCode string
		fmt.Scanln(&verificationCode)

		tok, err := c.AuthorizeToken(requestToken, verificationCode)
		if err != nil {
			log.Fatal(err)
		}
		*accessToken, *accessSecret = tok.Token, tok.Secret
		log.Printf("OK, got authorize token")
	}

	if *consumerKey == "" || *consumerSecret == "" || *accessToken == "" || *accessSecret == "" {
		if err := ioutil.WriteFile(
			*credentials,
			[]byte(strings.Join(
				[]string{
					*consumerKey,
					*consumerSecret,
					*accessToken,
					*accessSecret,
				},
				"\n",
			)),
			0600,
		); err != nil {
			log.Fatal(err)
		}
	}

	response, err := c.Get(
		"https://stream.twitter.com/1.1/statuses/sample.json",
		map[string]string{},
		&oauth.AccessToken{*accessToken, *accessSecret},
	)
	if err != nil {
		log.Fatal(err)
	}
	defer response.Body.Close()

	io.Copy(os.Stdout, response.Body)
}
