path = ARGV.length > 0 ? ARGV[0] : '/usr/share/dict/words'

max = 0

buckets =  Hash.new {|h,k| h[k]=[]}

File.read(path).split("\n").each do |w|
  next if  w[0] < 'a'
  l = w.length
  cps = w.codepoints.map{|c| c - 97}
  s = cps.inject(:+)

  key = (((((((s << 5) + l) << 5) + cps.first) << 5) + (cps[1]||0)) << 5) + (cps[2]||0)
  max = key if key > max
  buckets[key] << w
end

#p buckets.keys.length
#sizes = buckets.values.map{|b| b.size}
#puts "max: #{sizes.max}"
#puts "average: #{sizes.inject(:+).to_f/sizes.count}"

#p max
puts %q(
bool test(char * word, size_t len) {
  long hash = 0;

  int sum = 0;
  size_t i;
  for (i =0;  word[i] != '\0'; i++) {
    sum += word[i] - 'a';
  }
  hash = sum;
 // printf("sum: %lu\n", hash);

  hash = hash << 5;
  hash += len;
  //printf("len: %lu\n", hash);

  hash = hash << 5;
  hash += (word[0]-'a');
 // printf("first: %lu\n", hash);

  hash = hash << 5;
  if (len > 1) hash += word[1]-'a';
 // printf("second: %lu", hash);


  hash = hash << 5;
  if (len > 2) hash += word[2]-'a';
 // printf("%s(%zu): %lu", word,len, hash);
)


puts "switch(hash) {"
buckets.each do |k,b|
puts "case #{k}:"
b.each do |w|
  puts %Q(  if (!strcmp(word,"#{w}")) return 1;)
end
#puts %Q(  return 0;)
end

puts "default:"
puts "  return 0;"
puts "}" # end case

puts "}" # end function

