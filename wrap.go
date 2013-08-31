package main

import (
	"fmt"
	"os/exec"
	"strconv"
	"strings"
)

func rhymez(word string) ([]string, error) {
	buf, _ := exec.Command("rhyme-0.9/rhyme.bash", word).CombinedOutput()
	rhymes := []string{}
	for _, rhyme := range strings.Split(strings.TrimSpace(string(buf)), " ") {
		if idx := strings.Index(rhyme, "("); idx >= 0 {
			rhyme = rhyme[:idx]
		}
		if strings.HasSuffix(rhyme, ".") {
			rhyme = rhyme[:len(rhyme)-1]
		}
		if rhyme == word {
			continue
		}
		if len(rhyme) < 2 {
			continue
		}
		rhymes = append(rhymes, rhyme)
	}
	if len(rhymes) <= 0 {
		return []string{}, fmt.Errorf("no rhymes")
	}
	return rhymes, nil
}

func syllablez(s string) (int, error) {
	total := 0
	for _, word := range strings.Split(s, " ") {
		buf, _ := exec.Command("rhyme-0.9/rhyme.bash", "-s", word).CombinedOutput()
		out := strings.TrimSpace(string(buf))
		toks := strings.Split(out, " ")
		if len(toks) != 2 {
			return 0, fmt.Errorf("bad output '%s'", out)
		}
		n, err := strconv.ParseInt(toks[0], 10, 64)
		if err != nil {
			return 0, err
		}
		total += int(n)
	}
	return total, nil
}
