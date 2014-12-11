task :default => "build"

desc "Build Application"
task :build do |t|
  Dir.chdir("Script") do
    sh "root -b -q -l loadClasses.C"
  end
end

desc "Event Display"
task "show" do |t|
  Dir.chdir("Script") do
    sh "root -l show.C"
  end
end

desc "Test"
task "test" do |t|
  Dir.chdir("Script") do
    # provide your own test case
    sh "root -b -q -l test.C"
  end
end