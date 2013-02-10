package main

import (
	"testing"
)

func TestPair(t *testing.T) {
	rh := NewRhyme(*rhymeBin)
	pb := NewPoetryBox(rh)

	for i, text := range []string{
		`RT @ConnieRodarte: “@exquisitekarixo: Never did I think you'd have this huge affect on me.”`,
		`@jackkingram its all about mellow heather 4`,
		`All I wanna know is if the sandwich is for free`,
		`Respect is for those who deserve it, not for those who demand it.`,
	} {
		tweet := Tweet{
			ID:   uint64(i),
			Text: text,
		}

		t.Logf("%s", tweet.Text)
		couplet, ok := pb.Feed(&tweet)
		if ok {
			t.Logf("MATCH!")
			t.Logf("    %s", couplet.First.Text)
			t.Logf("    %s", couplet.Second.Text)
			t.Logf(" ")
		} else {
			t.Logf("no match")
			t.Logf(" ")
		}
	}
}
