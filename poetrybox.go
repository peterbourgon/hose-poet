package main

import (
	"fmt"
	"strings"
	"unicode"
)

type PoetryBox struct {
	m map[string]Tweet // encoded meter/rhyme scheme: most recent Tweet
}

func NewPoetryBox() *PoetryBox {
	return &PoetryBox{
		m: map[string]Tweet{},
	}
}

type Couplet struct {
	First  Tweet
	Second Tweet
}

func (pb *PoetryBox) Feed(tweet Tweet) (Couplet, bool) {
	classification, err := tweet.Classify()
	if err != nil {
		return Couplet{}, false
	}

	if tweet0, ok := pb.m[classification]; ok {
		return Couplet{tweet0, tweet}, true
	}
	pb.m[classification] = tweet
	return Couplet{}, false
}

//
//
//

func (t *Tweet) Classify() (string, error) {
	wordCount := 0
	for _, word := range Tokenize(t.Text) {
		word := Clean(word)
		if RejectText(word) {
			return "", fmt.Errorf("unclassifiable")
		}
		if RejectWord(word) {
			continue
		}
		wordCount++
	}
	return fmt.Sprintf("%d", wordCount), nil
}

func Tokenize(text string) []string {
	return strings.Split(strings.ToLower(text), " ")
}

func Clean(word string) string {
	return strings.NewReplacer(
		`.`, ``,
		`!`, ``,
		`?`, ``,
		`:`, ``,
		`,`, ``,
		`“`, ``,
		`”`, ``,
	).Replace(strings.TrimSpace(word))
}

func RejectWord(word string) bool {
	for _, bad := range []string{
		"",
		"rt",
	} {
		if word == bad {
			return true
		}
	}

	for _, prefix := range []string{
		"http",
		"@",
	} {
		if strings.Index(word, prefix) >= 0 {
			return true
		}
	}

	allPunct := true
	for _, r := range word {
		allPunct = allPunct && unicode.IsPunct(r)
	}
	if allPunct {
		return true
	}

	return false
}

func RejectText(word string) bool {
	if len([]byte(word)) > len([]rune(word)) {
		return true
	}
	for _, invalidating := range []string{
		`%`,
	} {
		if strings.Index(word, invalidating) >= 0 {
			return true
		}
	}
	return false
}
