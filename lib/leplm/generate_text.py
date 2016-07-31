with open("lep.wc", 'r') as word_counts:

    for line in word_counts:

	count = int(line.split("\t").pop(1))
	word = line.split("\t").pop(0)

	for i in xrange(0, count):

	    print "<s> " + word + " </s>"
