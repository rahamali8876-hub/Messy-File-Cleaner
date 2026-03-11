i want to scan basedirectory and subdirectories for
files with a specific extension like .jpg, .jpeg, .png, .gif .pdf and etc.
i wannat create folders based on extestions like jpeg_jan_2002 foldername based one extestions and created / updated / accessed time of files and move files to those folders based on their extensions and created / updated / accessed time.
i want all works done seperately in a thread and i want to show progress of this work in main thread.
and also i want to stop this work if i want to stop it later, same for loging the progress and errors in a log file.
progress, errors, cancelations, and logging should be thread-safe and should not cause any race conditions.
and same here i will add features slowly slowly.

1. Read file metadata (via platform)
2. Decide folder (via core)
3. Create folder if needed (via platform)
4. Move file (via platform)
5. Update progress counter (atomic)
6. Log result (thread-safe)

🏛 Where Each Concern Belongs
Concern Layer
Extension filtering Core
Folder name formatting Core
Progress counters System
Threadpool orchestration System
Cancellation flag System
Logging queue System
File IO Platform
Time retrieval Platform
