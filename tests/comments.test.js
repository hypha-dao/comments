const { loadConfig, Blockchain } = require("@klevoya/hydra");

const config = loadConfig("hydra.yml");

describe("comments", () => {
  const blockchain = new Blockchain(config);
  const comments = blockchain.createAccount('comments');
  const dao = blockchain.createAccount('dao');

  beforeAll(async () => {
    comments.setContract(blockchain.contractTemplates['comments']);
    comments.updateAuth(`active`, `owner`, {
      accounts: [
        {
          permission: {
            actor: comments.accountName,
            permission: `eosio.code`
          },
          weight: 1
        }
      ]
    });
  });

  beforeEach(async () => {
    comments.resetTables();
  });

  it("add section creates the section", async () => {
    await comments.contract.addsection({
      scope: dao.accountName,
      section: "my.section",
      author: "user1",
    }, [
      {
        actor: dao.accountName, 
        permission: 'active'
      }
    ]);

    expect(comments.getTableRowsScoped(`sections`)[dao.accountName]).toMatchObject([
      {
        section: "my.section",
        author: "user1",
        comments: "0"
      }
    ]);
  });

  it("Can not add the section twice", async () => {
    await comments.contract.addsection({
      scope: dao.accountName,
      section: "my.section",
      author: "user1",
    }, [
      {
        actor: dao.accountName, 
        permission: 'active'
      }
    ]);

    await expect(comments.contract.addsection({
      scope: dao.accountName,
      section: "my.section",
      author: "user1",
    }, [
      {
        actor: dao.accountName, 
        permission: 'active'
      }
    ])).rejects.toThrowError(/Section already exists/i);
  });
});
