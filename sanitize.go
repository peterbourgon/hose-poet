package main

import (
	"fmt"
	"strings"
)

func destupify(s string) (string, error) {
	words := strings.Split(strings.ToLower(s), " ")
	if words[0] == "rt" {
		return "", fmt.Errorf("is RT")
	}

	detokenizedWords := []string{}
	for _, word := range words {
		if word == "" {
			continue
		}
		switch word[0] {
		case '#':
			return "", fmt.Errorf("contains hashtag")
		case '@':
			return "", fmt.Errorf("contains @-user")
		}
		if strings.HasPrefix(word, "http") {
			return "", fmt.Errorf("contains link")
		}
		detokenizedWords = append(detokenizedWords, word)
	}
	words = detokenizedWords

	simpleWords := []string{}
	for _, word := range words {
		simpleWord := []rune{}
		for _, r := range word {
			switch r {
			case '\'':
				continue
			}
			if r < 'A' || r > 'z' {
				continue
			}
			simpleWord = append(simpleWord, r)
		}
		simpleWords = append(simpleWords, string(simpleWord))
	}
	words = simpleWords

	return strings.TrimSpace(strings.Join(words, " ")), nil
}
