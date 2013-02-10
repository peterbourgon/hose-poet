package main

import (
	"bufio"
	"bytes"
	"fmt"
	"os/exec"
	"strconv"
	"strings"
	"unicode"
)

type Rhyme struct {
	bin string
}

func NewRhyme(bin string) *Rhyme {
	return &Rhyme{
		bin: bin,
	}
}

func (r *Rhyme) Classify(tweet Tweet) (string, error) {
	textSyllables, lastWord := 0, ""
	for _, word := range Tokenize(tweet.Text) {
		word := Clean(word)
		if RejectText(word) {
			return "", fmt.Errorf("unclassifiable")
		}
		if RejectWord(word) {
			continue
		}

		wordSyllables, err := r.Syllables(word)
		if err != nil {
			continue
		}

		textSyllables += wordSyllables
		lastWord = word
	}

	if textSyllables <= 0 || len(lastWord) <= 1 {
		return "", fmt.Errorf("unclassifiable")
	}

	bucket, err := r.Bucket(lastWord)
	if err != nil {
		return "", fmt.Errorf("unclassifiable")
	}

	return fmt.Sprintf("%d-%s", textSyllables, bucket), nil
}

//
//
//

func (r *Rhyme) Syllables(word string) (int, error) {
	buf, err := exec.Command(r.bin, "--syllable", word).CombinedOutput()
	if err != nil {
		return 0, err
	}

	toks := strings.Split(string(buf), " ")
	if strings.Index(toks[0], "-") >= 0 {
		toks = strings.Split(toks[0], "-")[1:]
	}
	i, err := strconv.ParseInt(toks[0], 10, 64)
	if err != nil {
		return 0, err
	}

	return int(i), nil
}

func (r *Rhyme) Bucket(word string) (string, error) {
	buf, err := exec.Command(r.bin, word).CombinedOutput()
	if err != nil {
		return "", err
	}
	firstWord, err := bufio.NewReader(bytes.NewBuffer(buf)).ReadString(' ')
	if err != nil {
		return "", err
	}
	return strings.TrimSpace(firstWord), nil
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

//
//
//

type StringSet struct {
	m map[string]struct{}
}

func NewStringSet() *StringSet               { return &StringSet{map[string]struct{}{}} }
func (ss *StringSet) Add(s string)           { ss.m[s] = struct{}{} }
func (ss *StringSet) Contains(s string) bool { _, ok := ss.m[s]; return ok }
