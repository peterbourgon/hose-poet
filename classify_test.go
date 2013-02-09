package main

import (
	"testing"
)

var (
	testText = []string{
		`RT @ConnieRodarte: “@exquisitekarixo: Never did I think you'd have this huge affect on me.”`,
		`@jackkingram its all about mellow heather 4`,
		`RT @9mmiock: これちょっと失敗 http://t.co/lIitRHfP`,
		`📢เป็นพ่อค้าหรือแม่ค้าดีสับสน 🙈 เรามีตัวตนอยู่จริงนะค่ะ ไม่โกงไม่หนี แน่นอนจ้า ☝☝☝😸 @ 📢Preg.oSecret ' http://t.co/qoQ654gO`,
		`keenbean69 just a normal Saturday for our house #DayDrinking http://t.co/8zGjhXUp`,
		`#SocialMedia Lessons from the Open Source Movement - http://t.co/Oomnd3WL`,
		`RT @FFAmericans: Federal control of public education and interference in private marketplace is the (cont) http://t.co/IHrbMfn2`,
		`All I wanna know is what finesse means.`,
		`Ihop with my jig😘🍩☕ @brookepospichal`,
		`meterte a facebook y solo poder decir jajajajajajajajajajajajaja!`,
		`Respect is for those who deserve it, not for those who demand it.`,
		`RT @sweetthang_xoxo: No kids ! So that means a QUIET house .. #winning`,
		`Мне тебя не выкричать, не вымучать ,не вымыть... / Мне тебя?.. Нет...больше никогда. / Мне тебя теперь даже не вы... http://t.co/vZPqLmGn`,
		`RT @lmaginations: Remember me? I was your friend when you were single.`,
		`ギノ可愛いカレーうどんかわいい http://t.co/r7vmLKJv`,
		`@studunmow wish I had a Kat to kick lol`,
		`que feo es no hacer nada por la vida necesito plata   plata`,
		`You're so stupid. Lmfao.`,
		`@YPanterova ты у меня с сума сошла малыш а?`,
		`RT @JayParmar14: Back in the UK, India is one crazy place!`,
		`RT @RisasLocas: —Escribí un libro. —¿Y por qué has dibujado un dedo en la primera página? —Es el índice.`,
		`Be careful what you wish for cause you just might get it...`,
		`Keshi not bothered Pitroipa will face Nigeria, says Emenike not 100% fit http://t.co/OoqcFe9T`,
		`5 Games You Should Play This Weekend http://t.co/nQcXUP6K`,
	}
)

func TestClassify(t *testing.T) {
	for _, text := range testText {
		t.Logf(text)
		for _, word := range Tokenize(text) {
			cleanWord := Clean(word)

			flag1 := "✔"
			if RejectText(cleanWord) {
				flag1 = "✘"
			}

			flag2 := "✔"
			if RejectWord(cleanWord) {
				flag2 = "✗"
			}

			t.Logf("  %s %s %s (%s)", flag1, flag2, cleanWord, word)
		}

		tweet := Tweet{
			Text: text,
		}
		if classification, err := tweet.Classify(); err == nil {
			t.Logf("= %s", classification)
		} else {
			t.Logf("= (invalid)")
		}
		t.Logf(" ")
	}
}

func TestPair(t *testing.T) {
	pb := NewPoetryBox()
	for i, text := range testText {
		tweet := Tweet{
			Text: text,
		}

		couplet, ok := pb.Feed(tweet)
		if ok {
			t.Logf("%d: MATCH!", i+1)
			t.Logf("    %s", couplet.First.Text)
			t.Logf("    %s", couplet.Second.Text)
			t.Logf(" ")
		} else {
			t.Logf("%d: no match", i+1)
		}
	}
}
