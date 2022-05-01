const node_url = 'https://iotdr008319m1.herokuapp.com/'

async function init() {
    topBar()
    buildList()
};

const buildList = async () => {
    const container = document.querySelector('.containerList')

    const esp = await fetch(node_url+'esps',{headers : {
        'Access-Control-Allow-Origin': '*'
    }}).then(res => res.json()).then(res => res.result)

    esp.forEach(element => {
        let div = document.createElement('div')
        div.id = element.ident
        div.classList.add('card')
        
        let label = document.createElement('div')
        label.innerHTML = element.ident
        let input = document.createElement('input')

        if(element.surname) {input.value = element.surname}


        let btn = document.createElement('button')
        btn.innerHTML = 'Modifier'
        btn.onclick = () => {fetch(node_url+'esps/'+element._id,{
            method : 'PUT',
            headers: {
                'Accept': 'application/json',
                'Content-Type': 'application/json'
              },
            body : JSON.stringify({surname : input.value})})}

        div.appendChild(label)
        div.appendChild(input)
        div.appendChild(btn)
        container.appendChild(div)
    });
}


//assigns the onload event to the function init.
//=> When the onload event fires, the init function will be run. 
window.onload = init;


