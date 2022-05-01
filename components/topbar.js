const paths = [{name : 'Home', slug : '/'},{name : 'Carte', slug : '/Map'},{name : 'Liste', slug : '/espsConfig'}]

const topBar = () => {
    let topBar = document.querySelector('#topbar')

    let title = document.createElement("div")
    title.id = "title"
    title.innerHTML = "ESPs of Lucioles"

    let pathsContainer = document.createElement("div")
    pathsContainer.id = "pathContainer"

    paths.forEach(path => {
        let element = document.createElement("a")
        element.innerHTML = path.name
        element.href = path.slug
        pathsContainer.appendChild(element)
    })

    topBar.appendChild(title)
    topBar.appendChild(pathsContainer)
}
