Pour déployer via docker

0. se connecter : heroku container:login
1. build une image doncker : docker build . -t 'nom de l'image'
2. relier l'image avec l'application créé sur heroku : docker tag 'nom de l'image' registry.heroku.com/'nom de l'app heroku'/web
3. push l'image : docker push registry.heroku.com/'nom de l'app heroku'/web
4. réaliser la release : heroku container:release web -a 'nom de l'app heroku'

