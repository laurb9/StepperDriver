---
description: Steps to release a new version of the library
---

1. Check the current version in `library.properties`
   cat library.properties

2. Update `library.properties` version
   // Replace X.Y.Z with the new version
   sed -i '' 's/version=.*/version=X.Y.Z/' library.properties

3. Update `library.json` version
   // Replace X.Y.Z with the new version
   sed -i '' 's/"version": ".*"/"version": "X.Y.Z"/' library.json

4. Commit the version bump
   git add library.properties library.json
   git commit -m "Bump version to X.Y.Z"

5. Create a git tag
   git tag vX.Y.Z

6. Push changes and tags
   git push origin master
   git push origin vX.Y.Z
